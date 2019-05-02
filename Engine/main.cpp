#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <IL/il.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "tinyxml2.h"
using namespace tinyxml2;

int numModels = 0;
GLuint *vertices, *normals, *texCoords;

int total = 0;

GLdouble dist = 100, beta = M_PI_4, alpha = M_PI_4, xd = 0, zd = 0;

float accelerator = 1;

typedef std::tuple<float, float, float> vertice;
typedef std::tuple<float, float> v_2d;
typedef std::vector<vertice> Vertices;

typedef std::tuple<char, float, float, float, float> transformation;
typedef std::vector<transformation> Transformations;

std::vector<float*> luzes;

typedef struct curva {
	int valid;
	float time;
	Vertices pontos;
} Curva;

struct model {
	int indice;
	int numVertices;
	int texture;
	std::vector<vertice> vertices;
	std::vector<vertice> normals;
	std::vector<v_2d> texCoords;
} Model;

typedef struct group {
	Transformations trans;
	Curva c;
	std::vector<struct model> models;
	std::vector<struct group> subGroups;
} Group;

typedef std::vector<Group> Groups;

Groups allGroups;

void buildRotMatrix(float *x, float *y, float *z, float *m) {

	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}


void cross(float *a, float *b, float *res) {
	res[0] = a[1]*b[2] - a[2]*b[1];
	res[1] = a[2]*b[0] - a[0]*b[2];
	res[2] = a[0]*b[1] - a[1]*b[0];
}


void normalize(float *a) {
	float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0]/l;
	a[1] = a[1]/l;
	a[2] = a[2]/l;
}

float* multMatrixVector(float *m, float *v, float *res) {
	for (int j = 0; j < 4; j++) {
		res[j] = 0;
		for (int k = 0; k < 4; k++) {
			res[j] += v[k] * m[j * 4 + k];
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

v_2d extractVertice2d(std::string s) {
    std::string del = " ";
    float x, y;
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

    return v_2d(x, y);
}

model addVertices(std::ifstream &vertices) {
    char x[100];
	char y[100];
	model md;
	int numVertices = 0;

    while(vertices.getline(x, 100)) {
		vertices.getline(y, 100);
		if(strcmp(x, "TEXTURA:") == 0 || strcmp(y, "TEXTURA:") == 0) break;
		md.vertices.push_back(extractVertice(x));
		md.normals.push_back(extractVertice(y));
		numVertices++;
    }

	while (vertices.getline(x, 100)) {
		md.texCoords.push_back(extractVertice2d(x));
	}

	md.indice = numModels;
	md.numVertices = numVertices;
	return md;
}

int loadTexture(std::string s) {

	printf("Cheguei aqui2\n");
	unsigned int t, tw, th;
	unsigned char *texData;
	unsigned int texID;

	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilGenImages(1, &t);
	ilBindImage(t);
	ilLoadImage((ILstring)s.c_str());
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	texData = ilGetData();

	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
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

		else if(name == "models") {
			for(XMLElement *model = elem -> FirstChildElement("model"); model != nullptr; model = model -> NextSiblingElement("model")) {
				std::ifstream f;
				f.open(model->Attribute("file"));
				curG.models.push_back(addVertices(f));
				f.close();

				printf("%s\n", model -> Attribute("texture"));
				curG.models.at(numModels).texture = loadTexture(model->Attribute("texture"));

				numModels++;
			

				// COLOR !! ultimo elemento: 0 -> diffuse, 1 -> specular, 2 -> emissive, 3 -> ambient
				if(model -> FloatAttribute("diffR")) {
					transformation t (std::make_tuple('C', model -> FloatAttribute("diffR"), model -> FloatAttribute("diffG"), model -> FloatAttribute("diffB"), 0));
					curG.trans.push_back(t);
				}
				if(model -> FloatAttribute("specR")) {
					transformation t (std::make_tuple('C', model -> FloatAttribute("specR"), model -> FloatAttribute("specG"), model -> FloatAttribute("specB"), 1));
					curG.trans.push_back(t);
				}
				if(model -> FloatAttribute("emiR")) {
					transformation t (std::make_tuple('C', model -> FloatAttribute("emiR"), model -> FloatAttribute("emiG"), model -> FloatAttribute("emiB"), 2));
					curG.trans.push_back(t);
				}
				if(model -> FloatAttribute("ambR")) {
					transformation t (std::make_tuple('C', model -> FloatAttribute("ambR"), model -> FloatAttribute("ambG"), model -> FloatAttribute("ambB"), 3));
					curG.trans.push_back(t);
				}
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


void drawVBOs(std::vector<struct model> models) {
	for(model m: models){
		glBindTexture(GL_TEXTURE_2D, m.texture);

		glBindBuffer(GL_ARRAY_BUFFER, vertices[m.indice]);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, normals[m.indice]);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, texCoords[m.indice]);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, m.numVertices);

		glBindTexture(GL_TEXTURE_2D, 0);	
	}
}

void drawGroup(Group g) {

	glPushMatrix();

	float R, G, B, tipo;
	float a, x, y, z, time;
	int rots = 0;	

	// Transformations
	for(transformation tr: g.trans) {
		switch(std::get<0>(tr)) {
			case 'T':
				glTranslatef(std::get<1>(tr), std::get<2>(tr), std::get<3>(tr));
				break;

			case 'I':
				{
				time = std::get<1>(tr)/accelerator;
				a = fmod(float(glutGet(GLUT_ELAPSED_TIME))/1000, abs(time))*360/time;
				x = std::get<2>(tr);
				y = std::get<3>(tr);
				z = std::get<4>(tr);
				rots++;
				glRotatef(a, x, y, z);
				}
				break;

			case 'R':
				glRotatef(std::get<1>(tr), std::get<2>(tr), std::get<3>(tr), std::get<4>(tr));
				break;
				
			case 'S':
				glScalef(std::get<1>(tr), std::get<2>(tr), std::get<3>(tr));
				break;

			case 'C':
				{
					R = std::get<1>(tr);
					G = std::get<2>(tr);
					B = std::get<3>(tr);
					tipo = std::get<4>(tr);
					float cor[4] = {R, G, B, 1.0};
					if(tipo == 0)
						glMaterialfv(GL_FRONT, GL_DIFFUSE, cor);
					if(tipo == 1)
						glMaterialfv(GL_FRONT, GL_SPECULAR, cor);
					if(tipo == 2)
						glMaterialfv(GL_FRONT, GL_EMISSION, cor);
					if(tipo == 3)
						glMaterialfv(GL_FRONT, GL_AMBIENT, cor);
				
				}
				break;

			default:
				break;
		}
	}

	// Translação
	if(g.c.valid == 1) {
		float pos[3] = {0, 0, 0};
		float deriv[3] = {0, 0, 0};
		getGlobalCatmullRomPoint(float(float(glutGet(GLUT_ELAPSED_TIME))/1000/g.c.time), pos, deriv, g.c.pontos);
		glTranslatef(pos[0], pos[1], pos[2]);

		float z2[3];
		float y2[3] = {0,0,1};
		float mr[4][4];

		cross(deriv, y2, z2);
		normalize(z2);
		cross(z2, deriv, y2);
		normalize(y2);
		normalize(deriv);
		buildRotMatrix(deriv, y2, z2, (float *)mr);
		glMultMatrixf((float *)mr);
	}

    drawVBOs(g.models);

	if(rots >= 2) {
		glRotatef(-a, x, y, z);
	}

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

void fillVBOs(Group g) {
	for(auto m: g.models) {
		glBindBuffer(GL_ARRAY_BUFFER, vertices[m.indice]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m.numVertices * 3, m.vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, normals[m.indice]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m.numVertices * 3, m.normals.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, texCoords[m.indice]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m.numVertices * 2, m.texCoords.data(), GL_STATIC_DRAW);

		for(auto sg: g.subGroups) {
			fillVBOs(sg);
		}
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

	/*glLightfv(GL_LIGHT0, GL_POSITION, luzes.at(0));
	glLightfv(GL_LIGHT1, GL_POSITION, luzes.at(1));
	glLightfv(GL_LIGHT2, GL_POSITION, luzes.at(2));
	glLightfv(GL_LIGHT3, GL_POSITION, luzes.at(3));*/

	drawVertices();

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
    XMLElement *scene, *group, *lights, *light;
	int numLights = 0;

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
	//glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
/*
	// Luz
	float amb[4] = {0, 0, 0, 1};
	float diff[4] = {1, 1, 1, 1};
	lights = scene -> FirstChildElement("lights");
	light = lights -> FirstChildElement("light");
	for(; light != nullptr; light = light -> NextSiblingElement("light")) {
		const char *tipo = light -> Attribute("type");
		float X = light -> FloatAttribute("posX");
		float Y = light -> FloatAttribute("posY");
		float Z = light -> FloatAttribute("posZ");
		float t;

		if(strcmp(tipo, "POINT") == 0)
			t = 1;
		if(strcmp(tipo, "DIRECTIONAL") == 0)
			t = 0;
		if(strcmp(tipo, "SPOT") == 0) 
			t = -1;

		float pos[4] = {X, Y, Z, t};

		luzes.push_back(pos);

		numLights++;
	}

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glEnable(GL_LIGHT3);
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
		glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);
		glLightfv(GL_LIGHT2, GL_AMBIENT, amb);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, diff);
		glLightfv(GL_LIGHT3, GL_AMBIENT, amb);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, diff);
*/
    // Iterar pelos atributos group e adicioná-las a allGroups
    group = scene -> FirstChildElement("group");
    for(; group != nullptr; group = group -> NextSiblingElement("group")) {
        addGroup(group, nullptr);
		printf("Cheguei aqui\n");

    }

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Buffers
	vertices = (GLuint *)malloc(numModels * sizeof(GLuint));
	normals = (GLuint *)malloc(numModels * sizeof(GLuint));
	texCoords = (GLuint *)malloc(numModels * sizeof(GLuint));

	glGenBuffers(numModels, vertices);
	glGenBuffers(numModels, normals);
	glGenBuffers(numModels, texCoords);

	for(Group g: allGroups) {
		fillVBOs(g);
	}

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}