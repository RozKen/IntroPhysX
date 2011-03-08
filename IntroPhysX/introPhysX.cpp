/*************************************************************************
 *	introPhysX.cpp
 * @author - Kenichi Yorozu
 * @description - Simple Sample Code of PhysX
 ***********************************************************************/

#include <iostream>		//Used for Console Message I/O
#include <NxPhysics.h>	//Used for PhysX
//#include <GL/glut.h>		//Used for OpenGL (GLUT)
//#include "glutCallBacks.h"	//
//void mouse(int x,int y,int xx,int yy)
//{
//}
//int main()
//{
//	//glutMouseFunc(mouse);
//}
//#pragma comment(lib, "PhysXLoader.lib")
//��"�v���W�F�N�g�̃v���p�e�B>�\���v���p�e�B>�����J>����>�ǉ��̈ˑ��t�@�C��>PhysXLoader.lib"�̐ݒ肪���ĂȂ��Ƃ��ɕK�v.
	/*
	 *	Prototype Declaration
	 */
//void InitGLUT();
bool InitNx();
void CleanUpNx();
bool InitScene();
bool CreateGroundPlane();

	/*
	 *	Global Variables
	 */
NxPhysicsSDK* pPhysicsSDK = NULL;
NxScene* pScene = NULL;
NxVec3 DefaultGravity(0,-9.8,0);

	/**
	 * main
	 * @description - Entry Point for this Program
	 */
void main(){
	//InitGLUT();
	InitNx();
	InitScene();
	/*
	 *	Create Ground Plane
	 */
	{
		//actor Descriptor with Collection of Shapes.
		NxActorDesc	actorDesc;

		//Plane Shape Descriptor
		NxPlaneShapeDesc	planeDesc;
		//���ʕ�����: ax + by + cz + d = 0;
		planeDesc.normal = NxVec3(0, 0, 1);		//�ʂ̖@����Z������
		planeDesc.d = 0.0f;

		actorDesc.shapes.pushBack( &planeDesc );	//Actor��Plane��o�^
		
		//NxScene Creates Actor and Returns a Pointer.
		NxActor* pActor = pScene->createActor( actorDesc);
		pActor->userData = NULL;		//PhysX���Actor��(�Q�[���Ȃǂ�)�f�[�^���Actor�����т���
		
		//Set the parameters for the Default Material
		//Physics��"Material"�Ƃ͖ڂɌ�����\�ʍގ��ł͂Ȃ��C���̂̕����w�I������\��
		NxMaterial* defaultMaterial = pScene->getMaterialFromIndex( 0 );
		defaultMaterial->setRestitution( 0.3f );			//�����W��
		defaultMaterial->setStaticFriction( 0.5f );		//�Î~���C�W��
		defaultMaterial->setDynamicFriction( 0.5f );	//�����C�W��
	}
	/*
	 *	Create a Box Actor
	 */
	{
		//Create a Body Descriptor
		NxBodyDesc bodyDesc;
		bodyDesc.angularDamping = 0.5f;	//��]�����W��????
		bodyDesc.linearVelocity = NxVec3 (1, 0, 0); //�������x��X��������1
		
		//The Actor Descriptor
		NxActorDesc actorDesc;
		actorDesc.body = &bodyDesc;
		
		//Box Shape Descriptor
		NxBoxShapeDesc boxDesc;
		boxDesc.dimensions = NxVec3( 2.0f, 3.0f, 4.0f );	//4.0 x 6.0 x 8.0�̒�����
		actorDesc.shapes.pushBack( &boxDesc );	//Actor��Body��o�^
		actorDesc.density = 10.0f;	//���x10.0
		actorDesc.globalPose.t = NxVec3(10.0f, 10.0f, 10.0f);		//�����ʒu(10.0, 10.0, 10.0)
		//Set userData to NULL if you are not doing anyting with it.
		NxActor*pActor = pScene->createActor( actorDesc );
		pActor->userData = NULL;
	}

	/*
	 *	Simulate
	 */
	pScene->simulate( 1.0f/60.0f );

	//Render the Scene
	//// Setup default render states
	//glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_COLOR_MATERIAL);

	//// Setup lighting
	//glEnable(GL_LIGHTING);
	//float ambientColor[]	= { 0.0f, 0.1f, 0.2f, 0.0f };
	//float diffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		
	//float specularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		
	//float position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };		
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	//glLightfv(GL_LIGHT0, GL_POSITION, position);
	//glEnable(GL_LIGHT0);


	//Fetch the Results from the Simulation
	pScene ->flushStream();
	pScene->fetchResults( NX_RIGID_BODY_FINISHED, true );

	CleanUpNx();
}
	/**
	 * InitGLUT
	 */
//void InitGLUT(){
//	int argc;
//	char **argv;
//	glutInit(&argc, argv);
//	glutInitWindowSize(512, 512);
//	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
//	int mainHandle = glutCreateWindow("Simple Sample Code of PhysX");
//	glutSetWindow(mainHandle);
//	glutDisplayFunc(RenderCallback);
//	glutReshapeFunc(ReshapeCallback);
//	glutIdleFunc(IdleCallback);
//	glutKeyboardFunc(KeyboardCallback);
//	glutSpecialFunc(ArrowKeyCallback);
//	glutMouseFunc(MouseCallback);
//	glutMotionFunc(MotionCallback);
//	MotionCallback(0,0);
//	//atexit(CleanUpNx);
//}
	/**
	 *	InitNx()
	 *	@return bool - whether Physics SDK Initialization has done or not.
	 */
bool InitNx()
{
	bool initialized = false;
	pPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	if (pPhysicsSDK != NULL){
		initialized = true;
	}else{
		std::cout<<"Initialize Error: Cannot Create Physics SDK"<<std::endl;
	}
	return initialized;
}
	/**
	 *	InitScene()
	 * @return bool - whether Scene Initialization has done or not.
	 * @description - Initialize Scene.
	 */
bool InitScene(){
	bool sceneInit = false;
	//Create Scene Descripter
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = DefaultGravity;
	//Create Real Scene
	pScene = pPhysicsSDK->createScene(sceneDesc);
	if (pScene != NULL){
		sceneInit = true;
	}
	return sceneInit;
}
	/**
	 *	CleanUpNx()
	 *	@description - Clean Up Memory.
	 */
void CleanUpNx(){
	if(pPhysicsSDK != NULL){
		if( pScene != NULL ){
			pPhysicsSDK->releaseScene(*pScene);
			pScene = NULL;	//Release Scene Object
		}
		NxReleasePhysicsSDK( pPhysicsSDK );
		pPhysicsSDK = NULL;	//Release PhysicsSDK Object
	}
}
