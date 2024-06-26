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

struct p_2d {
	float x;
	float y;
};

struct CP {
	float x;
	float y;
	float z;
};

void cross(float *a, float *b, float *res) {
	res[0] = a[1]*b[2] - a[2]*b[1];
	res[1] = a[2]*b[0] - a[0]*b[2];
	res[2] = a[0]*b[1] - a[1]*b[0];
}

float* normalize(float *a) {
	float *b = (float *) malloc(sizeof(float) * 3);
	float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
	b[0] = a[0]/l;
	b[1] = a[1]/l;
	b[2] = a[2]/l;
	return b;
}

float* getVector(float *a, float *b) {
	float *res = (float *) malloc(sizeof(float) * 3);

	res[0] = b[0]-a[0];
	res[1] = b[1]-a[1];
	res[2] = b[2]-a[2];
	return res;
}

void drawVertex(float x, float y, float z) {
	f << x << " " << y << " " << z << "\n";
}

void drawVertexA(float *p){
	f << p[0] << " " << p[1] << " " << p[2] << "\n";
}

void drawPoints2D(std::vector<p_2d> texturas) {
	for(int i = 0; i < texturas.size(); i++) {
		f << texturas.at(i).x << " " << texturas.at(i).y << "\n";
	}
}

p_2d make_ponto(float x, float y) {
	p_2d ponto = {x, y};
	return ponto;
}

void drawPlane(float x, float z) {
	std::vector<p_2d> texturas;

	drawVertex(x/2, 0, z/2);
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(1,0));
	drawVertex(x/2, 0, -z/2);
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(1,1));
	drawVertex(-x/2, 0, z/2); 
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(0,0));

    drawVertex(-x/2, 0, z/2);
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(0,0));
    drawVertex(x/2, 0, -z/2);
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(1,1));
    drawVertex(-x/2, 0, -z/2);
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(0,1));

	drawVertex(-x/2, 0, z/2); 
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(0,0));
	drawVertex(x/2, 0, -z/2);
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(1,1));
	drawVertex(x/2, 0, z/2);
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(1,0));

    drawVertex(-x/2, 0, -z/2);
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(0,1));
    drawVertex(x/2, 0, -z/2);
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(1,1));
    drawVertex(-x/2, 0, z/2);
	drawVertex(0, 1, 0);
	texturas.push_back(make_ponto(0,0));

	f << "TEXTURA:\n";

	drawPoints2D(texturas);
}

void drawSkyBox(float x, float y, float z, int div = 1){
	std::vector<p_2d> texturas;
	
	float xOff = x /div;
	float zOff = z /div;
	float yOff = y /div;
	float x1 = -x /2;
	float y1 = -y /2;
	float z1 = -z /2;
	float x2 = x1;
	float y2 = y1;
	float z2 = z1;
	float nx = 1, ny = 1, nz = 1, nx2 = 0, ny2 = 0, nz2 = 0;
	float nxOff = 1.0 / div, nyOff = 1.0 / div, nzOff = 1.0 / div;

	for(float j = 0; j < div; j ++){
		x1 = -x /2;
		y1 = -y /2;
		z1 = -z /2;
		nx = 1;
		ny = 1;
		nz = 1;
		for(float i = 0; i < div; i ++){
			//Face superior
			drawVertex(x1, y/2, z2);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2, nx));
			drawVertex(x1+xOff, y/2, z2+zOff);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx-nxOff));
			drawVertex(x1+xOff, y/2, z2);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2, nx-nxOff));

			drawVertex(x1, y/2, z2);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2, nx));
			drawVertex(x1, y/2, z2+zOff);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx));
			drawVertex(x1+xOff, y/2, z2+zOff);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx-nxOff));

			//Face inferior
			drawVertex(x1, -y/2, z2);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2, nx));
			drawVertex(x1 + xOff, -y / 2, z2);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2, nx-nxOff));
			drawVertex(x1 + xOff, -y / 2, z2 + zOff);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx-nxOff));

			drawVertex(x1, -y/2, z2);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2, nx));
			drawVertex(x1+xOff, -y/2, z2+zOff);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx-nxOff));
			drawVertex(x1, -y/2, z2+zOff);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx));
			
			//Face Lateral 1
			drawVertex(x1, y2, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx, ny2));
			drawVertex(x1+xOff, y2+yOff, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx-nxOff, ny2+nyOff));
			drawVertex(x1, y2+yOff, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx, ny2+nyOff));

			drawVertex(x1, y2, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx, ny2));
			drawVertex(x1+xOff, y2, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx-nxOff, ny2));
			drawVertex(x1+xOff, y2+yOff, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx-nxOff, ny2+nyOff));

			//Face Lateral 2
			drawVertex(x1, y2, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx, ny2));
			drawVertex(x1, y2+yOff, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx, ny2+nyOff));
			drawVertex(x1+xOff, y2+yOff, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx-nxOff, ny2+nyOff));

			drawVertex(x1, y2, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx, ny2));
			drawVertex(x1+xOff, y2+yOff, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx-nxOff, ny2+nyOff));
			drawVertex(x1+xOff, y2, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx-nxOff, ny2));

			//Face Lateral 3
			drawVertex(x/2, y2, z1);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2));
			drawVertex(x/2, y2+yOff, z1);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2+nyOff));
			drawVertex(x/2, y2+yOff, z1+zOff);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz-nzOff, ny2+nyOff));

			drawVertex(x/2, y2, z1);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2));
			drawVertex(x/2, y2+yOff, z1+zOff);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz-zOff, ny2+yOff));
			drawVertex(x / 2, y2, z1 + zOff);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz-zOff, ny2));

			//Face Lateral 4
			drawVertex(-x/2, y2, z1);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2));
			drawVertex(-x/2, y2+yOff, z1+zOff);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz-zOff, ny2+nyOff));
			drawVertex(-x/2, y2+yOff, z1);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2+nyOff));

			drawVertex(-x/2, y2, z1);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2));
			drawVertex(-x/2, y2, z1+zOff);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz-zOff, ny2));
			drawVertex(-x/2, y2+yOff, z1+zOff);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz-zOff, ny2+nyOff));

			x1 += xOff;
			y1 += yOff;
			z1 += zOff;
			nx -= xOff;
			ny += yOff;
			nz -= zOff;
		}
		x2 += xOff;
		y2 += yOff;
		z2 += zOff;	
		nx2 += xOff;
		ny2 += yOff;
		nz2 += zOff;
	}

	f << "TEXTURA:\n";

	drawPoints2D(texturas);
}

void drawBox(float x, float y, float z, int div = 1){
	std::vector<p_2d> texturas;
	
	float xOff = x /div;
	float zOff = z /div;
	float yOff = y /div;
	float x1 = -x /2;
	float y1 = -y /2;
	float z1 = -z /2;
	float x2 = x1;
	float y2 = y1;
	float z2 = z1;
	float nx = 1, ny = 1, nz = 1, nx2 = 0, ny2 = 0, nz2 = 0;
	float nxOff = 1.0 / div, nyOff = 1.0 / div, nzOff = 1.0 / div;

	for(float j = 0; j < div; j ++){
		x1 = -x /2;
		y1 = -y /2;
		z1 = -z /2;
		nx = 1;
		ny = 1;
		nz = 1;
		for(float i = 0; i < div; i ++){
			//Face superior
			drawVertex(-x1, y/2, z2);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2, nx));
			drawVertex(-x1-xOff, y/2, z2);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2, nx-nxOff));
			drawVertex(-x1-xOff, y/2, z2+zOff);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx-nxOff));

			drawVertex(-x1, y/2, z2);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2, nx));
			drawVertex(-x1-xOff, y/2, z2+zOff);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx-nxOff));
			drawVertex(-x1, y/2, z2+zOff);
			drawVertex(0, 1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx));

			//Face inferior
			drawVertex(-x1, -y/2, z2);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2, nx));
			drawVertex(-x1 - xOff, -y / 2, z2 + zOff);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx-nxOff));
			drawVertex(-x1 - xOff, -y / 2, z2);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2, nx-nxOff));

			drawVertex(-x1, -y/2, z2);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2, nx));
			drawVertex(-x1, -y/2, z2+zOff);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx));
			drawVertex(-x1-xOff, -y/2, z2+zOff);
			drawVertex(0, -1, 0);
			texturas.push_back(make_ponto(nz2+nzOff, nx-nxOff));
			
			//Face Lateral 1
			drawVertex(-x1, y2, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx, ny2));
			drawVertex(-x1, y2+yOff, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx, ny2+nyOff));
			drawVertex(-x1-xOff, y2+yOff, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx-nxOff, ny2+nyOff));

			drawVertex(-x1, y2, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx, ny2));
			drawVertex(-x1-xOff, y2+yOff, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx-nxOff, ny2+nyOff));
			drawVertex(-x1-xOff, y2, z/2);
			drawVertex(0, 0, 1);
			texturas.push_back(make_ponto(nx-nxOff, ny2));

			//Face Lateral 2
			drawVertex(-x1, y2, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx, ny2));
			drawVertex(-x1-xOff, y2+yOff, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx-nxOff, ny2+nyOff));
			drawVertex(-x1, y2+yOff, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx, ny2+nyOff));

			drawVertex(-x1, y2, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx, ny2));
			drawVertex(-x1-xOff, y2, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx-nxOff, ny2));
			drawVertex(-x1-xOff, y2+yOff, -z/2);
			drawVertex(0, 0, -1);
			texturas.push_back(make_ponto(nx-nxOff, ny2+nyOff));

			//Face Lateral 3
			drawVertex(-x/2, y2, z1);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2));
			drawVertex(-x/2, y2+yOff, z1+zOff);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz-nzOff, ny2+nyOff));
			drawVertex(-x/2, y2+yOff, z1);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2+nyOff));

			drawVertex(-x/2, y2, z1);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2));
			drawVertex(-x / 2, y2, z1 + zOff);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz-zOff, ny2));
			drawVertex(-x/2, y2+yOff, z1+zOff);
			drawVertex(1, 0, 0);
			texturas.push_back(make_ponto(nz-zOff, ny2+yOff));

			//Face Lateral 4
			drawVertex(x/2, y2, z1);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2));
			drawVertex(x/2, y2+yOff, z1);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2+nyOff));
			drawVertex(x/2, y2+yOff, z1+zOff);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz-zOff, ny2+nyOff));

			drawVertex(x/2, y2, z1);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz, ny2));
			drawVertex(x/2, y2+yOff, z1+zOff);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz-zOff, ny2+nyOff));
			drawVertex(x/2, y2, z1+zOff);
			drawVertex(-1, 0, 0);
			texturas.push_back(make_ponto(nz-zOff, ny2));

			x1 += xOff;
			y1 += yOff;
			z1 += zOff;
			nx -= xOff;
			ny += yOff;
			nz -= zOff;
		}
		x2 += xOff;
		y2 += yOff;
		z2 += zOff;	
		nx2 += xOff;
		ny2 += yOff;
		nz2 += zOff;
	}

	f << "TEXTURA:\n";

	drawPoints2D(texturas);
}

void drawCone(float radius, float height, int slices, int stacks) {
	double sl = 2 * M_PI / slices;
	float r = radius/((float)stacks);
	float h = height/((float)stacks);
	float nr, nh;

	std::vector<p_2d> texturas;

	for (int i = 0; i < slices; i++) {
		drawVertex(0,0,0);
		drawVertex(0, -1, 0);
		drawVertex(radius * sin((i+1) * sl), 0, radius * cos((i+1) * sl));
		drawVertex(0, -1, 0);
		drawVertex(radius * sin(i*sl), 0, radius * cos(i*sl));
		drawVertex(0, -1, 0);
	}

	nr = radius - r;
	for (int i = 0; i < stacks; i++) {
		height = i * h;
		nh = (i+1) * h;
		float p1[3], p2[3], p3[3];

		for(float j = 0; j < slices; j++)
		{
			// Triangulas virados para cima
			float *res1, *res2, *res3;
			p1[0] = radius * sin(j*sl); p1[1] = height; p1[2] = radius * cos(j*sl);
			p2[0] = radius * sin((j+1)*sl); p2[1] = height; p2[2] = radius * cos((j+1)*sl);
			p3[0] = nr * sin((j + 1) * sl); p3[1] = nh; p3[2] = nr * cos((j + 1) * sl);
			
			drawVertexA(p1);
			p1[1] = h;
			drawVertexA(normalize(p1));
			drawVertexA(p2);
			p2[1] = h;
			drawVertexA(normalize(p2));
			drawVertexA(p3);
			p3[1] = h;
			drawVertexA(normalize(p3));

			//j += 0.5;
			// Triangulos virados para baixo
			p1[0] = nr * sin((j+1)*sl);  p1[1] = nh; p1[2] = nr * cos((j+1)*sl);
			p2[0] = nr*sin(j*sl); p2[1] = nh; p2[2] = nr*cos(j*sl);
			p3[0] = radius * sin(j * sl); p3[1] = height; p3[2] = radius * cos(j * sl);

			drawVertexA(p1);
			p1[1] = h;
			drawVertexA(normalize(p1));
			drawVertexA(p2);
			p2[1] = h;
			drawVertexA(normalize(p2));
			drawVertexA(p3);
			p3[1] = h;
			drawVertexA(normalize(p3));

			//j -= 0.5;
		}
		radius = nr;
		nr -= r;
	}

	f << "TEXTURA:\n";

	drawPoints2D(texturas);
}

void drawSphere(int radius, int slices, int stacks) {	
	double sl = (2.0 * M_PI) / slices;
	double beta = M_PI_2 / (stacks / 2);
	float nr, h, nh;
	float r = radius;
	std::vector<p_2d> texturas;
	float st = M_PI / stacks;
	float tex_offset = 0.5;

	for(int i = 0; i < (((float)stacks)/2); i++) {
		nr = radius * cos(beta*(i+1));
		h = radius * sin(beta*i);
		nh = radius * sin(beta*(i+1));
		float p1[3], p2[3], p3[3];

		for(float j = 0; j < slices; j++) {
			//Triangulos voltados para cima - metade superior
			p1[0] = r * sin(j*sl); p1[1] = h; p1[2] = r * cos(j*sl);
			texturas.push_back(make_ponto(j*sl / (2.0 * M_PI), tex_offset + i*st / M_PI));
			p2[0] = r * sin((j+1)*sl); p2[1] = h; p2[2] = r * cos((j+1)*sl);
			texturas.push_back(make_ponto((j+1)*sl / (2.0 * M_PI), tex_offset + i*st / M_PI));
			p3[0] = nr * sin((j+1)*sl); p3[1] = nh; p3[2] = nr * cos((j+1)*sl);
			texturas.push_back(make_ponto((j+1)*sl / (2.0 * M_PI), tex_offset + (i+1)*st / M_PI));

			drawVertexA(p1);
			drawVertexA(normalize(p1));
			drawVertexA(p2);
			drawVertexA(normalize(p2));
			drawVertexA(p3);
			drawVertexA(normalize(p3));
		
			// Triangulos voltados para cima - metade inferior
			p1[0] = r * sin(j*sl); p1[1] = -h; p1[2] = r * cos(j*sl);
			texturas.push_back(make_ponto(j*sl / (2.0 * M_PI), tex_offset - i*st / M_PI));
			p2[0] = nr * sin(j*sl); p2[1] = -nh; p2[2] = nr * cos(j*sl);
			texturas.push_back(make_ponto(j*sl / (2.0 * M_PI), tex_offset - (i+1)*st / M_PI));
			p3[0] = r * sin((j+1)*sl); p3[1] = -h; p3[2] = r * cos((j+1)*sl);
			texturas.push_back(make_ponto((j+1)*sl / (2.0 * M_PI), tex_offset - i*st / M_PI));

			drawVertexA(p1);
			drawVertexA(normalize(p1));
			drawVertexA(p2);
			drawVertexA(normalize(p2));
			drawVertexA(p3);
			drawVertexA(normalize(p3));

			//j += 0.5;
			// Triangulos voltados para baixo - metade superior
			p1[0] = nr * sin((j+1)*sl); p1[1] = nh; p1[2] = nr * cos((j+1)*sl);
			texturas.push_back(make_ponto((j+1)*sl / (2.0 * M_PI), tex_offset + (i+1)*st / M_PI));
			p2[0] = nr * sin(j*sl); p2[1] = nh; p2[2] = nr * cos(j*sl);
			texturas.push_back(make_ponto(j*sl / (2.0 * M_PI), tex_offset + (i+1)*st / M_PI));
			p3[0] = r * sin(j*sl); p3[1] = h; p3[2] = r * cos(j*sl);
			texturas.push_back(make_ponto(j*sl / (2.0 * M_PI), tex_offset + i*st / M_PI));

			drawVertexA(p1);
			drawVertexA(normalize(p1));
			drawVertexA(p2);
			drawVertexA(normalize(p2));
			drawVertexA(p3);
			drawVertexA(normalize(p3));

			// Triangulos voltados para baixo - metade inferior
			p1[0] = nr * sin((j+1)*sl); p1[1] = -nh; p1[2] = nr * cos((j+1)*sl);
			texturas.push_back(make_ponto((j+1)*sl / (2.0 * M_PI), tex_offset - (i+1)*st / M_PI));
			p2[0] = r * sin((j+1)*sl); p2[1] = -h; p2[2] = r*cos((j+1)*sl);
			texturas.push_back(make_ponto((j+1)*sl / (2.0 * M_PI), tex_offset - i*st / M_PI));
			p3[0] = nr * sin(j*sl); p3[1] = -nh; p3[2] = nr*cos(j*sl);
			texturas.push_back(make_ponto(j*sl / (2.0 * M_PI), tex_offset - (i+1)*st / M_PI));

			drawVertexA(p1);
			drawVertexA(normalize(p1));
			drawVertexA(p2);
			drawVertexA(normalize(p2));
			drawVertexA(p3);
			drawVertexA(normalize(p3));
			//j -= 0.5;
		}
		r = nr;
	}

	f << "TEXTURA:\n";

	drawPoints2D(texturas);
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

	// VER NORMAIS
	// estou a saltar as duas primeiras
	for(int i = 3; i < indexes.size(); i ++) {
		CP c1 = vertices.at(indexes.at(i-3));
		CP c2 = vertices.at(indexes.at(i-2));
		CP c3 = vertices.at(indexes.at(i-1));
		float p1[3] = {c1.x, c1.y, c1.z}, p2[3] = {c2.x, c2.y, c2.z}, p3[3] = {c3.x, c3.y, c3.z};
		
		drawVertex(c1.x, c1.y, c1.z);
		float *v1 = getVector(p1, p2);
		float *v2 = getVector(p1, p3);
		float n1[3];
		cross(v1, v2, n1);
		normalize(n1);
		drawVertexA(n1);

		drawVertex(c2.x, c2.y, c2.z);
		v1 = getVector(p2, p1);
		v2 = getVector(p2, p3);
		cross(v1, v2, n1);
		normalize(n1);
		drawVertexA(n1);

		drawVertex(c3.x, c3.y, c3.z);
		v1 = getVector(p3, p2);
		v2 = getVector(p3, p1);
		cross(v1, v2, n1);
		normalize(n1);
		drawVertexA(n1);
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
	} else if(strcmp(argv[1], "skybox") == 0) {
		if(argc == 6) {
			x = atoi(argv[2]);
			y = atoi(argv[3]);
			z = atoi(argv[4]);
			drawSkyBox(x,y,z);
		} else {
			printf("Número incorreto de argumentos\nUsage: ./generate skybox x y z\n");
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
