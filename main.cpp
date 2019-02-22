#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

GLdouble dist = 8, beta = M_PI_4, alpha = M_PI_4;

void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void drawCylinder(float radius, float height, int slices) {
	// put code to draw cylinder in here
	GLdouble alphaDelta = 2 * M_PI / slices;

	glBegin(GL_TRIANGLES);


	for (GLdouble i = 0; i < slices; i++) {
		// Festa dos tri�ngulos com os bicos para cima
		glColor3f(1, 1, 0);
		glVertex3f(radius*sin(i*alphaDelta), 0, radius*cos(i*alphaDelta));
		glVertex3f(radius*sin((i + 1)*alphaDelta), 0, radius*cos((i + 1)*alphaDelta));
		glVertex3f(0, height, 0);

		// Circunfer�ncia de baixo
		glColor3f(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX,rand()/(float)RAND_MAX);
		glVertex3f(0, 0, 0);
		glVertex3f(radius*sin(i*alphaDelta), 0, radius*cos(i*alphaDelta));
		glVertex3f(radius*sin((i - 1)*alphaDelta), 0, radius*cos((i - 1)*alphaDelta));

	}

	glEnd();
}

void drawPlane(int x, int z) {
    glBegin(GL_TRIANGLES);
        glColor3f(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX,rand()/(float)RAND_MAX);
        glVertex3f(0, 0, z / 2);
        glVertex3f(x / 2, 0, 0);
        glVertex3f(0, 0, -z / 2);

        glVertex3f(0,0,-z/2);
        glVertex3f(-x/2,0,0);
        glVertex3f(0,0,z/2);

		glVertex3f(0, 0, -z / 2);
        glVertex3f(x / 2, 0, 0);
        glVertex3f(0, 0, z / 2);
        
        glVertex3f(0,0,z/2);
        glVertex3f(-x/2,0,0);
        glVertex3f(0,0,-z/2);
        glEnd();

}


void renderScene(void) {
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(dist*cos(beta)*sin(alpha), dist*sin(beta), dist*cos(beta)*cos(alpha),
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	drawCylinder(1, 2, 100);
	drawPlane(10,10);

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {
	// put code to process regular keys in here
	float deltaToMove = 0.3;
	switch (c) {
	case 'q':
		dist += deltaToMove;
		break;

	case 'e':
		dist -= deltaToMove;
		break;

	default:
		return;
	}

	glutPostRedisplay();

}


void processSpecialKeys(int key, int xx, int yy) {
	// put code to process special keys in here
	float deltaToMove = 0.1;
	switch (key) {
	case GLUT_KEY_UP:
		beta += deltaToMove;
		break;

	case GLUT_KEY_DOWN:
		beta -= deltaToMove;
		break;

	case GLUT_KEY_LEFT:
		alpha -= deltaToMove;
		break;

	case GLUT_KEY_RIGHT:
		alpha += deltaToMove;
		break;

	default:
		return;
	}

	glutPostRedisplay();
}


int main(int argc, char **argv) {
	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI-UM");

	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
