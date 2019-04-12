#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

#define _USE_MATH_DEFINES
#include <math.h>

#include "tinyxml2.h"
using namespace tinyxml2;

GLdouble dist = 200, beta = M_PI_4, alpha = M_PI_4, xd = 0, zd = 0;

typedef std::tuple<float, float, float> vertice;
typedef std::vector<vertice> Vertices;

typedef std::tuple<char, float, float, float, float> transformation;
typedef std::vector<transformation> Transformations;

typedef struct group {
	Transformations trans;
	Curva c;
	Vertices v;
	std::vector<struct group> subGroups;
} Group;
typedef std::vector<Group> Groups;

Groups allGroups;

typedef struct curva {
	float time;
	Vertices pontos;
} Curva;


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
    y = atof(token.c_str());
    s.erase(0, p + del.length());

    // z
    p = s.find(del);
    token = s.substr(0, p);
    z = atof(token.c_str());

    return vertice(x, y, z);
}

void addVertices(std::ifstream &vertices, Group *g) {
    char x[100];
    while(vertices.getline(x, 100)) {
        g->v.push_back(extractVertice(x));
    }
}

void addGroup(XMLElement *group, Group *parent) {
	Group curG;
	
    for(XMLElement *elem = group -> FirstChildElement(); elem != nullptr; elem = elem -> NextSiblingElement()) {
		std::string name = elem->Value();

		if(name == "translate") {
			float time;
			if((time = elem->FloatAttribute("time"))) {
				Curva c;
				c.time = time;
				XMLElement *t = group -> FirstChildElement("translate");
				for(XMLElement *point = t -> FirstChildElement(); point != nullptr; point = point -> NextSiblingElement()) {
					vertice p (std::make_tuple(point->FloatAttribute("X"), point->FloatAttribute("Y"), point->FloatAttribute("Z")));
					c.pontos.push_back(p);
				}
				curG.c = c;
			} else {
				transformation t (std::make_tuple('T', elem->FloatAttribute("X"), elem->FloatAttribute("Y"), elem->FloatAttribute("Z"), 0));
				curG.trans.push_back(t);
			}
		}

		else if(name == "rotate") {
			transformation t (std::make_tuple('R', elem->FloatAttribute("A"), elem->FloatAttribute("X"), elem->FloatAttribute("Y"), elem->FloatAttribute("Z")));
			curG.trans.push_back(t);
		}

		else if(name == "scale") {
			transformation t (std::make_tuple('S', elem->FloatAttribute("X"), elem->FloatAttribute("Y"), elem->FloatAttribute("Z"), 0));
			curG.trans.push_back(t);
		}

		else if(name == "color") {
			transformation t (std::make_tuple('C', elem->FloatAttribute("R"), elem->FloatAttribute("G"), elem->FloatAttribute("B"), 0));
			curG.trans.push_back(t);
		}

		else if(name == "models") {
			for(XMLElement *model = elem -> FirstChildElement("model"); model != nullptr; model = model -> NextSiblingElement("model")) {
				std::ifstream f;
				f.open(model->Attribute("file"));
				addVertices(f, &curG);
				f.close();
			}
		}

		else if(name == "group") {
			addGroup(elem, &curG);
		}
	}

	if (parent == nullptr) {
		allGroups.push_back(curG);
	} else {
		parent->subGroups.push_back(curG);
	}
}


void drawGroup(Group g) {
	glPushMatrix();

	float R, G, B;
	bool color = false;

	// Transformations
	for(transformation t: g.trans) {
		switch(std::get<0>(t)) {
			case 'T':
				glTranslatef(std::get<1>(t), std::get<2>(t), std::get<3>(t));
				break;

			case 'R':
				glRotatef(std::get<1>(t), std::get<2>(t), std::get<3>(t), std::get<4>(t));
				break;
				
			case 'S':
				glScalef(std::get<1>(t), std::get<2>(t), std::get<3>(t));
				break;

			case 'C':
				if(std::get<1>(t) != 0 || std::get<2>(t) != 0 || std::get<3>(t) != 0) {
					R = std::get<1>(t);
					G = std::get<2>(t);
					B = std::get<3>(t);
					color = true;
				}
				break;

			default:
				break;
		}
	}

	// Vertices
	glBegin(GL_TRIANGLES);
    for(vertice v : g.v) {
		if (!color) {
        	glColor3f(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		} else {
			float var = (rand() / (float) RAND_MAX) / 5;
			glColor3f(R + var, G + var, B + var);
		}
        glVertex3f(std::get<0>(v), std::get<1>(v), std::get<2>(v));
    }
    glEnd();

	for(Group sg: g.subGroups) {
		drawGroup(sg);
	}

	glPopMatrix();
}

void drawVertices() {
    for(Group g: allGroups) {
		drawGroup(g);
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

	glTranslatef(xd, 0, zd);

    drawVertices();

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {
	// put code to process regular keys in here
	float deltaToZoom = 0.3;
	float deltaToMove = 0.1;
	switch (c) {
		case 'w':
			xd -= deltaToMove;
			zd -= deltaToMove;
			break;

		case 'a':
			xd -= deltaToMove;
			zd += deltaToMove;
			break;

		case 's':
			xd += deltaToMove;
			zd += deltaToMove;
			break;

		case 'd':
			xd += deltaToMove;
			zd -= deltaToMove;
			break;

		case 'q':
			dist += deltaToZoom;
			break;

		case 'e':
			dist -= deltaToZoom;
			break;

		case 'f':
			glPolygonMode(GL_FRONT, GL_FILL);
			break;

		case 'l':
			glPolygonMode(GL_FRONT, GL_LINE);
			break;

		case 'p':
			glPolygonMode(GL_FRONT, GL_POINT);
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
    XMLElement *scene, *group;

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

    // Iterar pelos atributos group e adicioná-las a allGroups
    group = scene -> FirstChildElement("group");
    for(; group != nullptr; group = group -> NextSiblingElement("group")) {
        addGroup(group, nullptr);
    }

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("1Fase-Engine");

	// Required callback registry 
	glutDisplayFunc(renderScene);
    // glutIdleFunc(renderScene);
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