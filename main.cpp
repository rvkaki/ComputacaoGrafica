#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

ofstream f;

GLdouble dist = 8, beta = M_PI_4, alpha = M_PI_4;

int ac;
char **av = NULL;  

void drawVertex(float x, float y, float z) {
	f << x << " " << y << " " << z << "\n";
}

void usage(char *shape){
	if(strcmp(shape, "sphere") == 0)
		cout << "Usage: ./class3 sphere radius slices stacks\n";
	if(strcmp(shape, "cone") == 0)
		cout << "Usage: ./class3 cone radius height slices stacks\n";
	if(strcmp(shape, "plane") == 0)
		cout << "Usage: ./class3 plane x z\n";
	if(strcmp(shape, "box") == 0)
		cout << "Usage: ./class3 box x y z\n";
}

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

void drawPlane(int x, int z) {
    glBegin(GL_TRIANGLES);
        glColor3f(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX,rand()/(float)RAND_MAX);
        drawVertex(0, 0, z / 2);
        drawVertex(x / 2, 0, 0);
        drawVertex(0, 0, -z / 2);

        drawVertex(0,0,-z/2);
        drawVertex(-x/2,0,0);
        drawVertex(0,0,z/2);

		drawVertex(0, 0, -z / 2);
        drawVertex(x / 2, 0, 0);
        drawVertex(0, 0, z / 2);
        
        drawVertex(0,0,z/2);
        drawVertex(-x/2,0,0);
        drawVertex(0,0,-z/2);
        glEnd();

}

void drawBox(int x, int y, int z, int nr = 2){
	glBegin(GL_TRIANGLES);
		glColor3f(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX,rand()/(float)RAND_MAX);
		
		//Face cima
		drawVertex(x/2, y/2, z/2);
		drawVertex(x/2, y/2, -z/2);
		drawVertex(-x/2, y/2, -z/2);

		drawVertex(-x/2, y/2, -z/2);
		drawVertex(-x/2, y/2, z/2);
		drawVertex(x/2, y/2, z/2);

		//Face baixo
		drawVertex(x/2, -y/2, -z/2);
		drawVertex(x/2, -y/2, z/2);
		drawVertex(-x/2, -y/2, z/2);

		drawVertex(-x/2, -y/2, z/2);
		drawVertex(-x/2, -y/2, -z/2);
		drawVertex(x/2, -y/2, -z/2);

		//Face lateral 1
		drawVertex(x/2, -y/2, z/2);
		drawVertex(x/2, y/2, z/2);
		drawVertex(-x/2, y/2, z/2);

		drawVertex(-x/2, y/2, z/2);
		drawVertex(-x/2, -y/2, z/2);
		drawVertex(x/2, -y/2, z/2);

		//Face lateral 2
		drawVertex(-x/2, -y/2, z/2);
		drawVertex(-x/2, y/2, z/2);
		drawVertex(-x/2, y/2, -z/2);

		drawVertex(-x/2, y/2, -z/2);
		drawVertex(-x/2, -y/2, -z/2);
		drawVertex(-x/2, -y/2, z/2);
		
		//Face lateral 3
		drawVertex(x/2, -y/2, -z/2);
		drawVertex(x/2, y/2, -z/2);
		drawVertex(x/2, y/2, z/2);

		drawVertex(x/2, y/2, z/2);
		drawVertex(x/2, -y/2, z/2);
		drawVertex(x/2, -y/2, -z/2);

		//Face lateral 4
		drawVertex(-x/2, -y/2, -z/2);
		drawVertex(-x/2, y/2, -z/2);
		drawVertex(x/2, y/2, -z/2);

		drawVertex(x/2, y/2, -z/2);
		drawVertex(x/2, -y/2, -z/2);
		drawVertex(-x/2, -y/2, -z/2);

		glEnd();
}

void drawCone(float radius, float height, int slices, int stacks) {
	double sl = 2 * M_PI / slices;
	float r = radius/((float)stacks);
	float h = height/((float)stacks);

	for (int i = 0; i < slices; i++) {
		drawVertex(0,0,0);
		drawVertex(radius * sin(i+1) * sl, 0, radius * cos(i+1) * sl);
		drawVertex(radius * sin(i*sl), 0, radius * cos(i*sl));
	}

	float nr = radius - r;
	for (int i = 0; i < stacks; i++) {
		height = i * h;
		float nh = (i+1) * h;

		for(float j = 0.5*(i%2); j < slices + 0.5*(i%2); j++)
		{
			// Triangulas virados para cima
			drawVertex(radius * sin(i*sl), height, radius * cos(i*sl));
			drawVertex(radius * sin((i+1)*sl), height, radius * cos((i+1)*sl));
			drawVertex(nr * sin((i+0.5)*sl), nh, nr * cos((i+0.5)*sl));

			i += 0.5;
			// Triangulos virados para baixo
			drawVertex(nr * sin((i+1)*sl), nh, nr * cos((i+1)*sl));
			drawVertex(nr*sin((i+1)*sl), nh, nr*cos((i+1)*sl));
			drawVertex(radius * sin((i+0.5)*sl), height, radius * cos((i+0.5)*sl));
			i -= 0.5;
		}
		radius = nr;
		nr -= r;
	}
}

void drawSphere(int radius, int slices, int stacks) {
	double sl = 2 * M_PI / slices;
	double beta = M_PI_2 / (stacks / 2);
	float nr, h, nh;

	float r = radius;
	for(int i = 0; i < ((float)stacks)/2; i++) {
		r = nr;
		nr = radius * cos(beta*(i+1));
		h = radius * sin(beta*i);
		nh = radius * sin(beta*(i+1));

		for(int j = 0.5 * (i%2); j < slices + 0.5*(j%2); i++) {
			//Triangulos voltados para cima - metade superior
			drawVertex(r * sin(i*sl), h, r * cos(i*sl));
			drawVertex(r * sin((i+1)*sl), h, r * cos((i+1)*sl));
			drawVertex(nr * sin((i+0.5)*sl), nh, nr * cos((i+0.5)*sl));
		
			// Triangulos voltados para cima - metade inferior
			drawVertex(r*sin(i*sl), -h, r*cos(i*sl));
			drawVertex(nr*sin((i+0.5)*sl), -nh, nr*cos((i+0.5)*sl));
			drawVertex(r*sin((i+1)*sl), -h, r*cos((i+1)*sl));

			i += 0.5;
			// Triangulos voltados para baixo - metade superior
			drawVertex(nr * sin((i+1)*sl), nh, nr * cos((i+1)*sl));
			drawVertex(nr * sin(i*sl), nh, nr * cos(i*sl));
			drawVertex(r * sin((i+0.5)*sl), h, r * cos((i+0.5)*sl));

			// Triangulos voltados para baixo - metade inferior
			drawVertex(nr * sin((i+1)*sl), -nh, nr * cos((i+1)*sl));
			drawVertex(r * sin((i+0.5)*sl), -h, r*cos((i+0.5)*sl));
			drawVertex(nr * sin(i*sl), -nh, nr*cos(i*sl));
			i -= 0.5;
		}
	}
}

void renderScene() {
	int badUsage = 0;
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(dist*cos(beta)*sin(alpha), dist*sin(beta), dist*cos(beta)*cos(alpha),
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	if(strcmp(av[1], "cone") == 0){
		if(ac == 5)
			drawCone(atoi(av[2]), atoi(av[3]), atoi(av[4]));
		else
			badUsage = 1;
	}
	if(strcmp(av[1], "plane") == 0){
		if(ac == 4)
			drawPlane(atoi(av[2]),atoi(av[3]));
		else
			badUsage = 1;
	}
	if(strcmp(av[1], "box") == 0){
		if(ac == 5)
			drawBox(atoi(av[2]),atoi(av[3]),atoi(av[4]));
		else
			badUsage = 1;
	}

	if(badUsage)
		usage(av[1]);

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
	int x,y,z;
	int radius, height, stacks, slices;

	if (argc < 5) {
		printf("É necessário indicar o ficheiro de destino e a forma geométrica a ser desenhada\n");
		exit(EXIT_FAILURE);
	}

	f.open(argv[argc-1]);

	switch (string(argv[1]))
	{
		case "plane":
			if(argc != 5) {
				printf("Número incorreto de argumentos\n");
				usage("plane");
				exit(EXIT_FAILURE);
			}

			x = atoi(argv[2]);
			z = atoi(argv[3]);
			drawPlane(x,z); 
			break;

		case "box":
			if(argc != 7 || argc != 6) {
				printf("Número incorreto de argumentos\n");
				usage("box");
				exit(EXIT_FAILURE);
			}

			x = atoi(argv[2]);
			y = atoi(argv[3]);
			z = atoi(argv[4]);
			drawBox(x,y,z);
			break;

		case "cone":
			if(argc != 7) {
				printf("Número incorreto de argumentos\n");
				usage("cone");
				exit(EXIT_FAILURE);
			}

			radius = atoi(argv[2]);
			height = atoi(argv[3]);
			slices = atoi(argv[4]);
			stacks = atoi(argv[5]);
			drawCone(radius, height, slices, stacks);
			break;

		case "sphere":
			if(argc != 6) {
				printf("Número incorreto de argumentos\n");
				usage("sphere");
				exit(EXIT_FAILURE);
			}

			radius = atoi(argv[2]);
			slices = atoi(argv[3]);
			stacks = atoi(argv[4]);
			drawSphere(radius, slices, stacks);
			break;
	
		default:
			printf("Figura não suportada\n");
			exit(EXIT_FAILURE);
			break;
	}

	f.close();

	// init GLUT and the window
	glutInit(&argc, argv);
	ac = argc;
	av = argv;
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI-UM");

	// Required callback registry 
	glutIdleFunc(renderScene);
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
