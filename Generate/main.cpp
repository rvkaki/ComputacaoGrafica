#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

ofstream f; 

struct CP {
	float x;
	float y;
	float z;
};

void drawVertex(float x, float y, float z) {
	f << x << " " << y << " " << z << "\n";
}

void drawPlane(float x, float z) {
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
}

void drawBox(float x, float y, float z, int div = 1){
	float xOff = x /div;
	float zOff = z /div;
	float yOff = y /div;
	float x1 = -x /2;
	float y1 = -y /2;
	float z1 = -z /2;
	float x2 = x1;
	float y2 = y1;
	float z2 = z1;
	for(float j = 0; j < div; j ++){
		x1 = -x /2;
		y1 = -y /2;
		z1 = -z /2;
		for(float i = 0; i < div; i ++){
			//Face superior
			drawVertex(x1, y/2, z2);
			drawVertex(x1+xOff, y/2, z2+zOff);
			drawVertex(x1+xOff, y/2, z2);

			drawVertex(x1, y/2, z2);
			drawVertex(x1, y/2, z2+zOff);
			drawVertex(x1+xOff, y/2, z2+zOff);

			//Face inferior
			drawVertex(x1, -y/2, z2);
			drawVertex(x1+xOff, -y/2, z2);
			drawVertex(x1+xOff, -y/2, z2+zOff);

			drawVertex(x1, -y/2, z2);
			drawVertex(x1+xOff, -y/2, z2+zOff);
			drawVertex(x1, -y/2, z2+zOff);

			//Face Lateral 1
			drawVertex(x1, y2, z/2);
			drawVertex(x1+xOff, y2+yOff, z/2);
			drawVertex(x1, y2+yOff, z/2);

			drawVertex(x1, y2, z/2);
			drawVertex(x1+xOff, y2, z/2);
			drawVertex(x1+xOff, y2+yOff, z/2);

			//Face Lateral 2
			drawVertex(x1, y2, -z/2);
			drawVertex(x1, y2+yOff, -z/2);
			drawVertex(x1+xOff, y2+yOff, -z/2);

			drawVertex(x1, y2, -z/2);
			drawVertex(x1+xOff, y2+yOff, -z/2);
			drawVertex(x1+xOff, y2, -z/2);

			//Face Lateral 3
			drawVertex(x/2, y2, z1);
			drawVertex(x/2, y2+yOff, z1);
			drawVertex(x/2, y2+yOff, z1+zOff);

			drawVertex(x/2, y2, z1);
			drawVertex(x/2, y2+yOff, z1+zOff);
			drawVertex(x/2, y2, z1+zOff);

			//Face Lateral 4
			drawVertex(-x/2, y2, z1);
			drawVertex(-x/2, y2+yOff, z1+zOff);
			drawVertex(-x/2, y2+yOff, z1);

			drawVertex(-x/2, y2, z1);
			drawVertex(-x/2, y2, z1+zOff);
			drawVertex(-x/2, y2+yOff, z1+zOff);


			x1 += xOff;
			y1 += yOff;
			z1 += zOff;
		}
		x2 += xOff;
		y2 += yOff;
		z2 += zOff;	
	}
}

void drawCone(float radius, float height, int slices, int stacks) {
	double sl = 2 * M_PI / slices;
	float r = radius/((float)stacks);
	float h = height/((float)stacks);
	float nr, nh;

	for (int i = 0; i < slices; i++) {
		drawVertex(0,0,0);
		drawVertex(radius * sin((i+1) * sl), 0, radius * cos((i+1) * sl));
		drawVertex(radius * sin(i*sl), 0, radius * cos(i*sl));
	}

	nr = radius - r;
	for (int i = 0; i < stacks; i++) {
		height = i * h;
		nh = (i+1) * h;

		for(float j = 0.5*(i%2); j < slices + 0.5*(i%2); j++)
		{
			// Triangulas virados para cima
			drawVertex(radius * sin(j*sl), height, radius * cos(j*sl));
			drawVertex(radius * sin((j+1)*sl), height, radius * cos((j+1)*sl));
			drawVertex(nr * sin((j+0.5)*sl), nh, nr * cos((j+0.5)*sl));

			j += 0.5;
			// Triangulos virados para baixo
			drawVertex(nr * sin((j+1)*sl), nh, nr * cos((j+1)*sl));
			drawVertex(nr*sin(j*sl), nh, nr*cos(j*sl));
			drawVertex(radius * sin((j+0.5)*sl), height, radius * cos((j+0.5)*sl));
			j -= 0.5;
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

	for(int i = 0; i < (((float)stacks)/2); i++) {
		nr = radius * cos(beta*(i+1));
		h = radius * sin(beta*i);
		nh = radius * sin(beta*(i+1));

		for(float j = 0.5 * (i%2); j < slices + 0.5*(i%2); j++) {
			//Triangulos voltados para cima - metade superior
			drawVertex(r * sin(j*sl), h, r * cos(j*sl));
			drawVertex(r * sin((j+1)*sl), h, r * cos((j+1)*sl));
			drawVertex(nr * sin((j+0.5)*sl), nh, nr * cos((j+0.5)*sl));
		
			// Triangulos voltados para cima - metade inferior
			drawVertex(r*sin(j*sl), -h, r*cos(j*sl));
			drawVertex(nr*sin((j+0.5)*sl), -nh, nr*cos((j+0.5)*sl));
			drawVertex(r*sin((j+1)*sl), -h, r*cos((j+1)*sl));

			j += 0.5;
			// Triangulos voltados para baixo - metade superior
			drawVertex(nr * sin((j+1)*sl), nh, nr * cos((j+1)*sl));
			drawVertex(nr * sin(j*sl), nh, nr * cos(j*sl));
			drawVertex(r * sin((j+0.5)*sl), h, r * cos((j+0.5)*sl));

			// Triangulos voltados para baixo - metade inferior
			drawVertex(nr * sin((j+1)*sl), -nh, nr * cos((j+1)*sl));
			drawVertex(r * sin((j+0.5)*sl), -h, r*cos((j+0.5)*sl));
			drawVertex(nr * sin(j*sl), -nh, nr*cos(j*sl));
			j -= 0.5;
		}
		r = nr;
	}
}

float* multMatrixVector(float *m, float *v, float *res) {
	for (int j = 0; j < 4; j++) {
		res[j] = 0;
		for (int k = 0; k < 4; k++) {
			res[j] += v[j * 4 + k] * m[j + k * 4];
		}
	}
}

CP* multMatrixCPVector(CP *cp, float *v, CP *res) {
	for(int i = 0; i < 4; i++) {
		res[i].x = 0;
		res[i].y = 0;
		res[i].z = 0;
		for(int j = 0; j < 4; j++) {
			res[i].x += v[j] * cp[i * 4 + j].x;
			res[i].y += v[j] * cp[i * 4 + j].y;
			res[i].z += v[j] * cp[i * 4 + j].z;
		}
	}
}

void multVectorCP(CP *cp, float *m, CP res) {
	for(int i = 0; i < 4; i++) {
		res.x = cp[i].x * m[i];
		res.y = cp[i].y * m[i];
		res.z = cp[i].z * m[i];
	}
}

CP bezierPatch(CP *controlPoints, float u, float v) {

	float U[4] = {powf(u,3), powf(u,2), powf(u,1), 1};
	float *tmp;
	CP res;
	CP *cp;

	float m[4][4] = {	{-0.5f,  1.5f, -1.5f,  0.5f},
						{ 1.0f, -2.5f,  2.0f, -0.5f},
						{-0.5f,  0.0f,  0.5f,  0.0f},
						{ 0.0f,  1.0f,  0.0f,  0.0f}};

	float bV[4] = {powf(1-v, 3),  3 * v * powf(1-v, 2), 3 * powf(v, 2) * (1-v),  powf(v, 3)};

	multMatrixVector((float *)m, U, tmp);
	multMatrixCPVector(controlPoints, tmp, cp);
	multVectorCP(cp, (float *)bV, res);

	return res;
}

CP evalBezierCurve(CP *p, float &t) {
	float b[4];
	CP res;
	res.x = 0; res.y = 0; res.z = 0;
	int i;

	b[0] = powf(1-t, 3);
	b[1] = 3 * t * powf(1-t, 2);
	b[2] = 3 * (1-t) * powf(t, 2);
	b[3] = powf(t, 3);

	for(i = 0; i < 4; i++) {
		p[i].x = p[i].x * b[i];
		p[i].y = p[i].y * b[i];
		p[i].z = p[i].z * b[i];
	}


	for(i = 0; i < 4; i++) {
		res.x += p[i].x;
		res.y += p[i].y;
		res.z += p[i].z; 
	}

	return res;
}

CP evalBezierPatch(CP *controlPoints, float u, float v) 
{ 
    CP uC[4]; 
    for (int i = 0; i < 4; ++i) {
		uC[i] = evalBezierCurve(controlPoints + 4 * i, u);
	}
    return evalBezierCurve(uC, v); 
}

void drawBezier(char *patch_file, int tessellation) {
	std::ifstream patchFile;
	patchFile.open(patch_file);

	char *line = (char *) malloc(1000);

	patchFile.getline(line, 1000);
	int numPatches = atoi(line);
	
	int patches[numPatches][16];
	std::string delimiter = ", ";

	for(int i = 0; i < numPatches; i++) {		
		patchFile.getline(line, 1000);
		std::string sep(line);
		std::string num;
		int pos = 0, j = 0;
		while((pos = sep.find(delimiter)) != std::string::npos) {
			num = sep.substr(0, pos);
			patches[i][j] = stoi(num);
			sep.erase(0, pos + delimiter.length());
			j++;
		}
		patches[i][j] = stoi(sep);
	}
	
	patchFile.getline(line, 1000);
	int numCP = atoi(line);

	CP cp[numCP];

	for(int i = 0; i < numCP; i++) {
		patchFile.getline(line, 1000);
		std::string sep(line);
		std::string coord;
		int pos = 0;

		pos = sep.find(delimiter);
		coord = sep.substr(0, pos);
		cp[i].x = stof(coord);
		sep.erase(0, pos + delimiter.length());

		pos = sep.find(delimiter);
		coord = sep.substr(0, pos);
		cp[i].y = stof(coord);
		sep.erase(0, pos + delimiter.length());

		pos = sep.find(delimiter);
		coord = sep.substr(0, pos);
		cp[i].z = stof(coord);
		sep.erase(0, pos + delimiter.length());
	}

    CP P[(tessellation + 1) * (tessellation + 1)];
	CP controlPoints[16];
	CP pos;
	std::vector<CP> vertices;
	std::vector<int> indexes;

	for (int np = 0; np < numPatches; np++) { 
		
		for (int j = 0; j <= tessellation; j++) {

			for (int k = 0; k <= tessellation; k++) { 
				// set the control points for the current patch
				for (int i = 0; i < 16; ++i) {
					controlPoints[i].x = cp[patches[np][i]].x;
					controlPoints[i].y = cp[patches[np][i]].y;
					controlPoints[i].z = cp[patches[np][i]].z;
				}
				pos = evalBezierPatch(controlPoints, j / (float)tessellation, k / (float)tessellation);

				vertices.push_back(pos); 
			}
		}
	}

	for(int np = 0; np < numPatches; np++) {
		int patch = (tessellation + 1) * (tessellation + 1) * np;

		for(int i = 0; i < tessellation; i++) {
			for(int j = 0; j < tessellation; j++) {
				indexes.push_back(patch + (tessellation + 1) * i + j);
				indexes.push_back(patch + (tessellation + 1) * (i + 1) + j + 1);
				indexes.push_back(patch + (tessellation + 1) * i + j + 1);
			
				indexes.push_back(patch + (tessellation + 1) * i + j);
				indexes.push_back(patch + (tessellation + 1) * (i + 1) + j);
				indexes.push_back(patch + (tessellation + 1) * (i + 1) + j + 1);
			}
		}
	}

	for(int i: indexes) {
		CP c = vertices.at(i);
		drawVertex(c.x, c.y, c.z);
	}
}

int main(int argc, char **argv) {
	int x,y,z;
	int radius, height, stacks, slices;
	int tessellation;
	char *pf;

	if (argc < 5) {
		printf("É necessário indicar o ficheiro de destino e a forma geométrica a ser desenhada\n");
		exit(EXIT_FAILURE);
	}

	f.open(argv[argc-1]);

	if(strcmp(argv[1], "plane") == 0) {
		if(argc != 5) {
			printf("Número incorreto de argumentos\nUsage: ./generate plane x z\n");
			exit(EXIT_FAILURE);
		} else {
			x = atoi(argv[2]);
			z = atoi(argv[3]);
			drawPlane(x,z);
		}
	} else if(strcmp(argv[1], "box") == 0) {
		if(argc == 6) {
			x = atoi(argv[2]);
			y = atoi(argv[3]);
			z = atoi(argv[4]);
			drawBox(x,y,z);
		} else if(argc ==7) {
			x = atoi(argv[2]);
			y = atoi(argv[3]);
			z = atoi(argv[4]);
			int div = atoi(argv[5]);
			drawBox(x,y,z, div);
		} else {
			printf("Número incorreto de argumentos\nUsage: ./generate box x y z div\n");
			exit(EXIT_FAILURE);
		}
	} else if(strcmp(argv[1], "cone") == 0) { 
		if(argc != 7) {
			printf("Número incorreto de argumentos\nUsage: ./generate cone radius height slices stacks\n");
			exit(EXIT_FAILURE);
		} else {
			radius = atoi(argv[2]);
			height = atoi(argv[3]);
			slices = atoi(argv[4]);
			stacks = atoi(argv[5]);
			drawCone(radius, height, slices, stacks);
		}
	} else if(strcmp(argv[1], "sphere") == 0) {
		if(argc != 6) {
			printf("Número incorreto de argumentos\nUsage: ./generate sphere radius slices stacks\n");
			exit(EXIT_FAILURE);
		} else {
			radius = atoi(argv[2]);
			slices = atoi(argv[3]);
			stacks = atoi(argv[4]);
			drawSphere(radius, slices, stacks);
		}
	} else if(strcmp(argv[1], "bezier") == 0){
		if(argc != 5) {
			printf("Número incorreto de argumentos\nUsage: ./generate bezier <patch file> tessellation\n");
			exit(EXIT_FAILURE);
		} else {
			pf = argv[2];
			tessellation = atoi(argv[3]);
			drawBezier(pf, tessellation);
		}
		
	
	} else {
		printf("Figura não suportada\n");
		exit(EXIT_FAILURE);
	}

	f.close();

	return 1;
}
