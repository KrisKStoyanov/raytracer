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

	std::vector<glm::vec3> ModelMeshVertices;
	std::vector<glm::vec3> ModelMeshNormals;

	//loadOBJ("../teapot_simple.obj", ModelMeshVertices, ModelMeshNormals);

	//Mesh* TeapotModelMesh = new Mesh(ModelMeshVertices, ModelMeshNormals);

	Sphere* RedSphere = new Sphere(glm::vec3(0, 0, -20), 4, glm::vec3(1, 0.32, 0.36), glm::vec3(1, 0.32, 0.36), glm::vec3(0.5,0.5,0.5));
	Sphere* YellowSphere = new Sphere(glm::vec3(5, -1, -15), 2, glm::vec3(0.9, 0.76, 0.46), glm::vec3(0.9, 0.76, 0.46), glm::vec3(0.5, 0.5, 0.5));
	Sphere* LightBlueSphere = new Sphere(glm::vec3(5, 0, -25), 3, glm::vec3(0.65, 0.77, 0.97), glm::vec3(0.65, 0.77, 0.97), glm::vec3(0.5, 0.5, 0.5));
	Sphere* LightGreySphere = new Sphere(glm::vec3(-5.5, 0, -15), 3, glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.5, 0.5, 0.5));

	Plane* TestPlane = new Plane(glm::vec3(0, -10004, -20), glm::vec3(0, 1, 0), glm::vec3(0,0,0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	//Triangle* TestTriangle = new Triangle(glm::vec3(0, 1, -2), glm::vec3(-1.9, -1, -2), glm::vec3(1.6, -0.5, -2), glm::vec4(0.5,0.5,0, 1));	

	std::vector<Shape*> Shapes;
	Shapes.push_back(RedSphere);
	Shapes.push_back(YellowSphere);
	Shapes.push_back(LightBlueSphere);
	Shapes.push_back(LightGreySphere);
	Shapes.push_back(TestPlane);
	//Shapes.push_back(TestTriangle);
	//Shapes.push_back(TeapotModelMesh);
	
	if (RTracer.Init("Rayspace", 800, 600, SDL_RENDERER_ACCELERATED)) {
		Camera* MainCamera = new Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), 1.0f, 0.25f);
		Light* PointLight = new Light(glm::vec3(0, 20, 0), glm::vec3(1.0f, 1.0f, 1.0f));
		RTracer.Configure(Shapes, MainCamera, glm::vec3(0.1f, 0.1f, 0.1f), PointLight);
		RTracer.Start();
	}
	else {
		return -1;
	}

	RTracer.Deactivate();

	return 0;
}