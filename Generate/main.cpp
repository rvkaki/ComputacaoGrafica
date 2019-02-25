#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

ofstream f; 

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

int main(int argc, char **argv) {
	int x,y,z;
	int radius, height, stacks, slices;

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
	} else {
		printf("Figura não suportada\n");
		exit(EXIT_FAILURE);
	}

	f.close();

	return 1;
}
