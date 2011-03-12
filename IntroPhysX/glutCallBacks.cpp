#include <iostream>
#include <NxPhysics.h>	//Used for PhysX
#include <GL/glut.h>		//Used for OpenGL (GLUT)
//glut.hを後ろに置かないと，exit関数についてエラーが生じる.
#pragma comment(lib, "glut32.lib")
// Rendering
static NxVec3	gEye(50.0f, 50.0f, 50.0f);	//Cameraの視点
static NxVec3	gDir(-1.0f,-1.0f,-1.0f);			//Cameraの向く方向
static NxVec3	gViewY;								//
static int		gMouseX = 0;							//マウスポインタの位置X
static int		gMouseY = 0;							//マウスポインタの位置Y
static unsigned char gMouseButton[3] = {0};	//マウスのボタンの押下状態

extern NxScene* pScene;

static void MyGLInit(){
	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	// Setup lighting
	glEnable(GL_LIGHTING);
	float ambientColor[]	= { 0.1f, 0.1f, 0.1f, 0.0f };
	float diffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		
	float specularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		
	float position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };		
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);
	
	glClearColor(0.5, 0.5, 0.5, 1.0);		//背景色を設定
}

static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:			//ESCキーが押されたら
			exit(0);			//プログラムを終了する
			break;
		//case GLUT_KEY_UP:	case '8':	gEye += gDir*2.0f; break;
		//case GLUT_KEY_DOWN: case '2':	gEye -= gDir*2.0f; break;
		//case GLUT_KEY_LEFT:	case '4':	gEye -= gViewY*2.0f; break;
		//case GLUT_KEY_RIGHT: case '6':	gEye += gViewY*2.0f; break;
	}
}

static void ArrowKeyCallback(int key, int x, int y)
{
	KeyboardCallback(key,x,y);
}

static void MouseCallback(int button, int state, int x, int y)
{
	gMouseX = x;
	gMouseY = y;
	switch(button){
		case GLUT_LEFT_BUTTON:
			gMouseButton[0] = ((GLUT_DOWN==state)?1:0);
			break;
		case GLUT_MIDDLE_BUTTON:
			gMouseButton[1] = ((GLUT_DOWN==state)?1:0);
			break;
		case GLUT_RIGHT_BUTTON:
			gMouseButton[2] = ((GLUT_DOWN==state)?1:0);
			break;
		default:
			break;
	}
	glutPostRedisplay();
}

static void MotionCallback(int x, int y)
{
	int dx = gMouseX - x;
	int dy = gMouseY - y;
	
	gDir.normalize();		//カメラの視線ベクトルを正規化
	gViewY.cross(gDir, NxVec3(0,1,0));	//

	if( gMouseButton[0] && gMouseButton[1] ){
		//Zoom: Left + Center Buttons Drag
		gEye -= gDir * 0.5f * dy;
	}else{
		if( gMouseButton[0] ){
			//Rotate: Left Button Drag
			NxQuat qx(NxPiF32 * dx * 10/ 180.0f, NxVec3(0,1,0));
			qx.rotate(gDir);
			NxQuat qy(NxPiF32 * dy * 10/ 180.0f, gViewY);
			qy.rotate(gDir);
		}else if( gMouseButton[1] ){
			//Move: Center Button Drag
			gEye += 0.1f * (gViewY * dx - NxVec3(0, 1, 0) * dy);
		}
	}
	gMouseX = x;
	gMouseY = y;
	glutPostRedisplay();
}

static void RenderCallback()
{
	if(pScene == NULL){
#ifdef _DEBUG
		std::cout << "pScene == NULL" << std::endl;
#endif //_DEBUG
		return;
	}
	
	// Start simulation (non blocking)
	pScene->simulate(1.0f/60.0f);
	
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Setup projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(gEye.x, gEye.y, gEye.z, gEye.x + gDir.x, gEye.y + gDir.y, gEye.z + gDir.z, 0.0f, 1.0f, 0.0f);

	// Setup modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Render all actors
	int nbActors = pScene->getNbActors();
	NxActor** actors = pScene->getActors();
	while(nbActors--)
	{
		NxActor* actor = *actors++;
		if(actor->userData == NULL){
			// draw grid
			glBegin(GL_LINES);
				int y = 0;
				for(int i=-100;i<=100;++i) {
					glVertex3f(i*10,y,-1000);
					glVertex3f(i*10,y,1000);

					glVertex3f(1000,y,i*10);
					glVertex3f(-1000,y,i*10);
				}
			glEnd();
		}

		// Render actor
		glPushMatrix();
		float glMat[16];
		actor->getGlobalPose().getColumnMajor44(glMat);
		glMultMatrixf(glMat);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glutSolidCube(size_t(actor->userData)*10.0f);
		glPopMatrix();

		// Render shadow
		glPushMatrix();
		const static float shadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
		glMultMatrixf(shadowMat);
		glMultMatrixf(glMat);
		glDisable(GL_LIGHTING);
		glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
		glutSolidCube(size_t(actor->userData)*10.0f);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}

	// Fetch simulation results
	pScene->flushStream();
	pScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

	glutSwapBuffers();
}

static void ReshapeCallback(int width, int height)
{
	glViewport(0, 0, width, height);
}

static void IdleCallback()
{
	glutPostRedisplay();
}