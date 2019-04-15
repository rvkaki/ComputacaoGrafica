#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

#define _USE_MATH_DEFINES
#include <math.h>

#include "tinyxml2.h"
using namespace tinyxml2;

float *pts;  
GLuint buffers[1];

int total = 0, indice = 0;

GLdouble dist = 100, beta = M_PI_4, alpha = M_PI_4, xd = 0, zd = 0;

float accelerator = 1;

typedef std::tuple<float, float, float> vertice;
typedef std::vector<vertice> Vertices;

typedef std::tuple<char, float, float, float, float> transformation;
typedef std::vector<transformation> Transformations;

typedef struct curva {
	int valid;
	float time;
	Vertices pontos;
} Curva;

typedef struct group {
	Transformations trans;
	Curva c;
	Vertices v;
	std::vector<struct group> subGroups;
} Group;
typedef std::vector<Group> Groups;

Groups allGroups;

float* multMatrixVector(float *m, float *v, float *res) {
	for (int j = 0; j < 4; j++) {
		res[j] = 0;
		for (int k = 0; k < 4; k++) {
			res[j] += v[j * 4 + k] * m[j + k * 4];
		}
	}
}

void multVectorVector(float v1[4], float v2[4], float res) {
	res = 0;
	for (int j = 0; j < 4; ++j) {
		res += v1[j] * v2[j];
	}
}


void multMatrixMatrix(float m1[4][4], float m2[4][4], float res[4][4]) {
	float aux[4];
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			for(int k = 0; k < 4; k++) {
				aux[k] = m2[k][i];
			}
		multVectorVector(m1[i], aux, res[i][j]);
		}
	}
}

void getCatmullRomPoint(float t, vertice v0, vertice v1, vertice v2, vertice v3, float *pos, float *deriv) {
	
	// T matrix
	float T[4] = {powf(t,3), powf(t,2), t, 1};
	float T1[4] = {3*powf(t,2), 2*t, 1, 0};


	// catmull-rom matrix
	float m[4][4] = {	{-0.5f,  1.5f, -1.5f,  0.5f},
						{ 1.0f, -2.5f,  2.0f, -0.5f},
						{-0.5f,  0.0f,  0.5f,  0.0f},
						{ 0.0f,  1.0f,  0.0f,  0.0f}};
			

	// Compute A = M * P   
	// i = 0
	float p1[4] = {std::get<0>(v0), std::get<0>(v1), std::get<0>(v2), std::get<0>(v3)};
	float a1[4];
	multMatrixVector((float*)m, p1, a1);
	pos[0] = T[0]*a1[0] + T[1]*a1[1] + T[2]*a1[2] + T[3]*a1[3];
	deriv[0] = T1[0]*a1[0] + T1[1]*a1[1] + T1[2]*a1[2] + T1[3]*a1[3];

	// i = 1
	float p2[4] = {std::get<1>(v0), std::get<1>(v1), std::get<1>(v2), std::get<1>(v3)};
	float a2[4];
	multMatrixVector((float*)m, p2, a2);
	pos[1] = T[0]*a2[0] + T[1]*a2[1] + T[2]*a2[2] + T[3]*a2[3];
	deriv[1] = T1[0]*a2[0] + T1[1]*a2[1] + T1[2]*a2[2] + T1[3]*a2[3];

	// i = 2
	float p3[4] = {std::get<2>(v0), std::get<2>(v1), std::get<2>(v2), std::get<2>(v3)};
	float a3[4];
	multMatrixVector((float*)m, p3, a3);
	pos[2] = T[0]*a3[0] + T[1]*a3[1] + T[2]*a3[2] + T[3]*a3[3];
	deriv[2] = T1[0]*a3[0] + T1[1]*a3[1] + T1[2]*a3[2] + T1[3]*a3[3];
}


// given  global t, returns the point in the curve
void getGlobalCatmullRomPoint(float gt, float *pos, float *deriv, Vertices pontos) {
	int POINT_COUNT = pontos.size();
	float t = gt * POINT_COUNT; // this is the real global t
	int index = floor(t);  // which segment
	t = t - index; // where within  the segment

	// indices store the points
	int indices[4]; 
	indices[0] = (index + POINT_COUNT-1)%POINT_COUNT;	
	indices[1] = (indices[0]+1)%POINT_COUNT;
	indices[2] = (indices[1]+1)%POINT_COUNT; 
	indices[3] = (indices[2]+1)%POINT_COUNT;

	getCatmullRomPoint(t, pontos[indices[0]], pontos[indices[1]], pontos[indices[2]], pontos[indices[3]], pos, deriv);
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
		total += 3;
    }
}

void addGroup(XMLElement *group, Group *parent) {
	Group curG;
	
    for(XMLElement *elem = group -> FirstChildElement(); elem != nullptr; elem = elem -> NextSiblingElement()) {
		std::string name = elem->Value();

		if(name == "translate") {
			Curva c;
			float time;
			if((time = elem -> FloatAttribute("time"))) {
				curG.c.valid = 1;
				curG.c.time = time;

				for(XMLElement *point = elem -> FirstChildElement(); point != nullptr; point = point -> NextSiblingElement()) {
					vertice p (std::make_tuple(point->FloatAttribute("X"), point->FloatAttribute("Y"), point->FloatAttribute("Z")));
					curG.c.pontos.push_back(p);
				}
			} else {
				curG.c.valid = 0;
				transformation t (std::make_tuple('T', elem->FloatAttribute("X"), elem->FloatAttribute("Y"), elem->FloatAttribute("Z"), 0));
				curG.trans.push_back(t);
			}
		}

		else if(name == "rotate") {
			float time;
			if((time = elem -> FloatAttribute("time"))) {
				transformation t (std::make_tuple('I', time, elem -> FloatAttribute("X"), elem -> FloatAttribute("Y"), elem -> FloatAttribute("Z")));
				curG.trans.push_back(t);
			} else {
				transformation t (std::make_tuple('R', elem -> FloatAttribute("A"), elem -> FloatAttribute("X"), elem -> FloatAttribute("Y"), elem -> FloatAttribute("Z")));
				curG.trans.push_back(t);
			}
		}

		else if(name == "scale") {
			transformation t (std::make_tuple('S', elem -> FloatAttribute("X"), elem -> FloatAttribute("Y"), elem -> FloatAttribute("Z"), 0));
			curG.trans.push_back(t);
		}

		else if(name == "color") {
			transformation t (std::make_tuple('C', elem -> FloatAttribute("R"), elem -> FloatAttribute("G"), elem -> FloatAttribute("B"), 0));
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
		parent -> subGroups.push_back(curG);
	}
}


void drawGroup(Group g, float m[4][4]) {

	pts = (float*)malloc(total * sizeof(float));
	//float m[4][4];
	//glPushMatrix();

	float R, G, B, a;
	bool color = false;
	float x, y, z, time;
	float res[4][4];
	

	// Transformations
	for(transformation tr: g.trans) {
		switch(std::get<0>(tr)) {
			case 'T':
				//glTranslatef(std::get<1>(tr), std::get<2>(tr), std::get<3>(tr));
				{
				float trans[4][4] = {{1, 0, 0, std::get<1>(tr)},
									 {0, 1, 0, std::get<2>(tr)},
									 {0, 0, 1, std::get<3>(tr)},
									 {0, 0, 0, 1}};
				
				multMatrixMatrix(m, trans, res);
				m = res;
				}
				break;

			case 'I':
				{
				time = std::get<1>(tr)/accelerator;
				a = fmod(float(glutGet(GLUT_ELAPSED_TIME))/1000, abs(time))*360/time;
				x = std::get<2>(tr);
				y = std::get<3>(tr);
				z = std::get<4>(tr);
				//glRotatef(angle, x, y, z);
				
				float trans[4][4] = {{powf(x,2) + (1-powf(x,2)) * cos(a), x*y*(1-cos(a))-z*sin(a), x*z*(1-cos(a))+y*sin(a), 0},
									 {x*y*(1-cos(a))+z*sin(a), powf(y,2) + (1-powf(y,2)) * cos(a), y*z*(1-cos(a))-x*sin(a), 0},
									 {x*z*(1-cos(a))-y*sin(a), y*z*(1-cos(a))+x*sin(a), powf(z,2) + (1-powf(z,2)) * cos(a), 0},
									 {0, 0, 0, 1}};
				multMatrixMatrix(m, trans, res);
				m = res;
				}
				break;

			case 'R':
				//glRotatef(std::get<1>(tr), std::get<2>(tr), std::get<3>(tr), std::get<4>(tr));
				{
				a = std::get<1>(tr);
				x = std::get<2>(tr);
				y = std::get<3>(tr);
				z = std::get<4>(tr);
				float trans[4][4] = {{powf(x,2) + (1-powf(x,2)) * cos(a), x*y*(1-cos(a))-z*sin(a), x*z*(1-cos(a))+y*sin(a), 0},
									 {x*y*(1-cos(a))+z*sin(a), powf(y,2) + (1-powf(y,2)) * cos(a), y*z*(1-cos(a))-x*sin(a), 0},
									 {x*z*(1-cos(a))-y*sin(a), y*z*(1-cos(a))+x*sin(a), powf(z,2) + (1-powf(z,2)) * cos(a), 0},
									 {0, 0, 0, 1}};
				multMatrixMatrix(m, trans, res);
				m = res;
				}
				break;
				
			case 'S':
				//glScalef(std::get<1>(tr), std::get<2>(tr), std::get<3>(tr));
				{
				float trans[4][4] = {{std::get<1>(tr), 0, 0, 0},
									 {0, std::get<2>(tr), 0, 0},
									 {0, 0, std::get<3>(tr), 0},
									 {0, 0, 0, 1}};
				multMatrixMatrix(m, trans, res);
				m = res;
				}
				break;

			case 'C':
				if(std::get<1>(tr) != 0 || std::get<2>(tr) != 0 || std::get<3>(tr) != 0) {
					R = std::get<1>(tr);
					G = std::get<2>(tr);
					B = std::get<3>(tr);
					color = true;
				}
				break;

			default:
				break;
		}
	}


	// Translação
	if(g.c.valid == 1){
		float pos[3] = {0, 0, 0};
		float deriv[3] = {0, 0, 0};
		getGlobalCatmullRomPoint(glutGet(GLUT_ELAPSED_TIME), pos, deriv, g.c.pontos);
		//glTranslatef(pos[0], pos[1], pos[2]);
		
		float trans[4][4] = {{1, 0, 0, pos[0]},
							 {0, 1, 0, pos[1]},
							 {0, 0, 1, pos[2]},
							 {0, 0, 0, 1}};
		multMatrixMatrix(m, trans, res);
		m = res;
	}

	// Vertices
	//glBegin(GL_TRIANGLES);
    for(vertice v : g.v) {
		if (!color) {
        	glColor3f(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		} else {
			float var = rand() / (float) RAND_MAX / 5;
			glColor3f(R + var, G + var, B + var);
		}
        //glVertex3f(std::get<0>(v), std::get<1>(v), std::get<2>(v));
		
		pts[indice++] = std::get<0>(v);
		pts[indice++] = std::get<1>(v);
		pts[indice++] = std::get<2>(v);
		
    }
    //glEnd();

	float trans[4][4] = {{powf(x,2) + (1-powf(x,2)) * cos(-a), x*y*(1-cos(-a))-z*sin(-a), x*z*(1-cos(-a))+y*sin(-a), 0},
									 {x*y*(1-cos(-a))+z*sin(-a), powf(y,2) + (1-powf(y,2)) * cos(-a), y*z*(1-cos(-a))-x*sin(-a), 0},
									 {x*z*(1-cos(-a))-y*sin(-a), y*z*(1-cos(-a))+x*sin(-a), powf(z,2) + (1-powf(z,2)) * cos(-a), 0},
									 {0, 0, 0, 1}};
	multMatrixMatrix(m, trans, res);
	
	for(Group sg: g.subGroups) {
		drawGroup(sg, m);
	}

	//glPopMatrix();

}

void drawVertices() {
	indice = 0;
    for(Group g: allGroups) {
		float m[4][4] = {{1, 0, 0, 0},
						 {0, 1, 0, 0},
						 {0, 0, 1, 0},
						 {0, 0, 0, 1}};
		drawGroup(g, m);
	}
}

void renderScene() {
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(dist*cos(beta)*sin(alpha), dist*sin(beta), dist*cos(beta)*cos(alpha),
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);


	glColor3f(1,0,0);
	drawVertices();

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
    //drawVertices(t);

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {
	// put code to process regular keys in here
	float deltaToZoom = 0.5;
	float deltaToMove = 0.3;
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
	float deltaToMove = 0.3;
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

	printf("%d\n", total);
	
	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("3Fase-Engine");

	// Required callback registry 
	glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
	glewInit();

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_LINE);

	//Buffers
	glEnableClientState(GL_VERTEX_ARRAY);
	glGenBuffers(1, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pts), pts, GL_STATIC_DRAW);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}