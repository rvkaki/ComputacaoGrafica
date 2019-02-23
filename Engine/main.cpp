#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

#define _USE_MATH_DEFINES
#include <math.h>

#include "tinyxml2.h"
using namespace tinyxml2;

GLdouble dist = 8, beta = M_PI_4, alpha = M_PI_4;

typedef std::tuple<float, float, float> vertice;
std::vector<vertice> Vertices;

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

vertice extractVertice(std::string s) {
    std::string del = " ";
    float x, y, z;
    int p;
    std::string token;

    // x
    p = s.find(del);
    token = s.substr(0, p);
    x = atof(token.c_str());
    s.erase(0, p + del.length());

    // y
    p = s.find(del);
    token = s.substr(0, p);
    x = atof(token.c_str());
    s.erase(0, p + del.length());

    // z
    p = s.find(del);
    token = s.substr(0, p);
    x = atof(token.c_str());

    return vertice(x, y, z);
}

void addVertices(std::ifstream &vertices) {
    char x[100];
    while(vertices.getline(x, 100)) {
        Vertices.push_back(extractVertice(x));
    }
}

void drawVertices() {
    glBegin(GL_TRIANGLES);

    for(vertice v : Vertices) {
        glColor3f(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
        glVertex3f(std::get<0>(v), std::get<1>(v), std::get<0>(v));
    }

    glEnd();
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

    drawVertices();

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
	XMLDocument doc;
    XMLError e;
    XMLElement *scene, *model;

    if(argc != 2) {
        std::cout << "Forneça um ficheiro XLM\n";
        exit(EXIT_FAILURE);
    }

    // Carregar o ficheiro XML
    e = doc.LoadFile(argv[1]);
    if (e != XML_SUCCESS) {
        std::cout << e << "Forneça um ficheiro XML válido\n";
        exit(EXIT_FAILURE);
    }

    // Guardar o atributo scene
    scene = doc.FirstChildElement("scene");
    if (!scene) {
        std::cout << "Formato inválido\n";
        exit(EXIT_FAILURE);
    }

    // Guardar o atributo model
    model = scene -> FirstChildElement("model");
    for(; model != nullptr; model = model -> NextSiblingElement()) {
        std::ifstream f;
        f.open(model -> Attribute("file"));
        addVertices(f);
        f.close();
    }

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("1Fase-Engine");

	// Required callback registry 
	glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
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