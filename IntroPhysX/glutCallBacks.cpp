#include <iostream>
#include <NxPhysics.h>	//Used for PhysX
#include <GL/glut.h>		//Used for OpenGL (GLUT)
//glut.hを後ろに置かないと，exit関数についてエラーが生じる.
#pragma comment(lib, "glut32.lib")
// Rendering
static NxVec3	gEye(50.0f, 50.0f, 50.0f);
static NxVec3	gDir(-1.0f,-1.0f,-1.0f);
static NxVec3	gViewY;
static int		gMouseX = 0;
static int		gMouseY = 0;

extern NxScene* pScene;

static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:	
			exit(0); 
			break;
		case GLUT_KEY_UP:	case '8':	gEye += gDir*2.0f; break;
		case GLUT_KEY_DOWN: case '2':	gEye -= gDir*2.0f; break;
		case GLUT_KEY_LEFT:	case '4':	gEye -= gViewY*2.0f; break;
		case GLUT_KEY_RIGHT: case '6':	gEye += gViewY*2.0f; break;
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
}

static void MotionCallback(int x, int y)
{
	int dx = gMouseX - x;
	int dy = gMouseY - y;
	
	gDir.normalize();
	gViewY.cross(gDir, NxVec3(0,1,0));

	NxQuat qx(NxPiF32 * dx * 20/ 180.0f, NxVec3(0,1,0));
	qx.rotate(gDir);
	NxQuat qy(NxPiF32 * dy * 20/ 180.0f, gViewY);
	qy.rotate(gDir);

	gMouseX = x;
	gMouseY = y;
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
				for(int i=-10;i<=10;++i) {
					glVertex3f(i*10,y,-100);
					glVertex3f(i*10,y,100);

					glVertex3f(100,y,i*10);
					glVertex3f(-100,y,i*10);
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