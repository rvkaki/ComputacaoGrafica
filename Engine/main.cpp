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

void drawPlane(int x, int z) {
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

void drawBox(int x, int y, int z, int nr = 2){
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
}

void drawCone(float radius, float height, int slices, int stacks) {
	double sl = 2 * M_PI / slices;
	float r = radius/((float)stacks);
	float h = height/((float)stacks);
	float nr, nh;

	for (int i = 0; i < slices; i++) {
		drawVertex(0,0,0);
		drawVertex(radius * sin(i+1) * sl, 0, radius * cos(i+1) * sl);
		drawVertex(radius * sin(i*sl), 0, radius * cos(i*sl));
	}

	nr = radius - r;
	for (int i = 0; i < stacks; i++) {
		height = i * h;
		nh = (i+1) * h;

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
			printf("Número incorreto de argumentos\n");
			usage("plane");
			exit(EXIT_FAILURE);
		} else {
			x = atoi(argv[2]);
			z = atoi(argv[3]);
			drawPlane(x,z);
		}
	} else if(strcmp(argv[1], "box") == 0) {
		if(argc != 7 || argc != 6) {
			printf("Número incorreto de argumentos\n");
			usage("box");
			exit(EXIT_FAILURE);
		} else {
			x = atoi(argv[2]);
			y = atoi(argv[3]);
			z = atoi(argv[4]);
			drawBox(x,y,z);
		}
	} else if(strcmp(argv[1], "cone") == 0) { 
		if(argc != 7) {
			printf("Número incorreto de argumentos\n");
			usage("cone");
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
			printf("Número incorreto de argumentos\n");
			usage("sphere");
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
