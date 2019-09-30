#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "objloader.h"
#include "Raytracer.h"
#include "Camera.h"
#include "Mesh.h"

//const unsigned int SCREEN_WIDTH = 800;
//const unsigned int SCREEN_HEIGHT = 600;

//glm::vec3** ScreenImageBuffer;

Raytracer RTracer;

int main(int argc, char* argv[]) {

	//ScreenImageBuffer = new glm::vec3 * [SCREEN_HEIGHT];
	//for (int x = 0; x < SCREEN_HEIGHT; ++x) {
	//	ScreenImageBuffer[x] = new glm::vec3[SCREEN_WIDTH];
	//}

	std::vector<glm::vec3> TeapotMeshVertices;
	std::vector<glm::vec3> TeapotMeshNormals;

	std::vector<glm::vec3> CubeMeshVertices;
	std::vector<glm::vec3> CubeMeshNormals;

	loadOBJ("../teapot_simple_smooth.obj", TeapotMeshVertices, TeapotMeshNormals);
	loadOBJ("../cube_simple.obj", CubeMeshVertices, CubeMeshNormals);

	Mesh* TeapotModelMesh = new Mesh(TeapotMeshVertices, TeapotMeshNormals, glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 128);
	Mesh* CubeModelMesh = new Mesh(CubeMeshVertices, CubeMeshNormals, glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 128);

	Sphere* RedSphere = new Sphere(glm::vec3(0, 0, -20), 4, glm::vec3(1, 0.32, 0.36), glm::vec3(1, 0.32, 0.36), glm::vec3(0.7f, 0.7f, 0.7f), 128);
	Sphere* YellowSphere = new Sphere(glm::vec3(5, -1, -15), 2, glm::vec3(0.9, 0.76, 0.46), glm::vec3(0.9, 0.76, 0.46), glm::vec3(0.7f, 0.7f, 0.7f), 128);
	Sphere* LightBlueSphere = new Sphere(glm::vec3(5, 0, -25), 3, glm::vec3(0.65, 0.77, 0.97), glm::vec3(0.65, 0.77, 0.97), glm::vec3(0.7f, 0.7f, 0.7f), 128);
	Sphere* LightGreySphere = new Sphere(glm::vec3(-5.5, 0, -15), 3, glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.7f, 0.7f, 0.7f), 128);

	Plane* Floor = new Plane(glm::vec3(0, -4, -20), glm::vec3(0, 1, 0), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), 128);
	//Triangle* Polygon = new Triangle(glm::vec3(0, 1, -2), glm::vec3(-1.9f, -1, -2), glm::vec3(1.6f, -0.5f, -2),
	//	glm::vec3(0.0f, 0.6f, 1.0f), glm::vec3(-0.4f,-0.4f,1.0f),glm::vec3(0.4f,-0.4f,1.0f),
	//	glm::vec3(1.0, 1.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.7f, 0.7f, 0.7f), 100);

	std::vector<Shape*> Shapes;
	Shapes.push_back(RedSphere);
	Shapes.push_back(YellowSphere);
	Shapes.push_back(LightBlueSphere);
	Shapes.push_back(LightGreySphere);
	Shapes.push_back(Floor);
	//Shapes.push_back(CubeModelMesh);
	//Shapes.push_back(TeapotModelMesh);

	//Shapes.push_back(Polygon);

	//delete TeapotModelMesh;
	//delete RedSphere;
	//delete YellowSphere;
	//delete LightBlueSphere;
	//delete LightGreySphere;
	//delete Floor;
	//delete Polygon;

	Camera* MainCamera = new Camera(glm::vec3(0, 0, 5), glm::vec3(0, 0, -1), 1.0f, 0.25f);
	Light* PointLight = new Light(glm::vec3(0, 3, 1), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::vec3 AmbientLightC = glm::vec3(0.1f, 0.1f, 0.1f);
	
	if (RTracer.Init("Rayspace", 800, 600, SDL_RENDERER_ACCELERATED)) {
		RTracer.Configure(Shapes, MainCamera, AmbientLightC, PointLight);
		RTracer.Start();
	}
	else {
		return -1;
	}

	RTracer.Deactivate();

	return 0;
}