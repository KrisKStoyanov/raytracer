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

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

glm::vec3** ScreenImageBuffer;

int main(int argc, char* argv[]) {

	//ScreenImageBuffer = new glm::vec3 * [SCREEN_HEIGHT];
	//for (int x = 0; x < SCREEN_HEIGHT; ++x) {
	//	ScreenImageBuffer[x] = new glm::vec3[SCREEN_WIDTH];
	//}

	Sphere* RedSphere = new Sphere(glm::vec3(0, 0, -20), glm::vec3(1, 0.32, 0.36), 4);
	Sphere* YellowSphere = new Sphere(glm::vec3(5, -1, -15), glm::vec3(0.9, 0.76, 0.46), 2);
	Sphere* LightBlueSphere = new Sphere(glm::vec3(5, 0, -25), glm::vec3(0.65, 0.77, 0.97), 3);
	Sphere* LightGreySphere = new Sphere(glm::vec3(-5.5, 0, -15), glm::vec3(0.9, 0.9, 0.9), 3);

	//Sphere* DarkGreySphere = new Sphere(glm::vec3(0, -10004, -20), glm::vec3(0.2, 0.2, 0.2), 10000);
	Plane* TestPlane = new Plane(glm::vec3(0, -10004, -20), glm::vec3(0, 1, 0), glm::vec3(0,0,0));
	Triangle* TestTriangle = new Triangle(glm::vec3(0, 1, -2), glm::vec3(-1.9, -1, -2), glm::vec3(1.6, -0.5, -2), glm::vec3(0.5,0.5,0));	

	float ScreenAspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << SCREEN_WIDTH << " " << SCREEN_HEIGHT << "\n255\n";
	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {

			float PixelNormalizedx = (x + 0.5) / SCREEN_WIDTH;
			float PixelNormalizedy = (y + 0.5) / SCREEN_HEIGHT;

			float PixelRemappedx = 2 * PixelNormalizedx - 1;
			float PixelRemappedy = 1 - 2 * PixelNormalizedy;

			PixelRemappedx = (2 * PixelNormalizedx - 1) * ScreenAspectRatio;
			PixelRemappedy = 1 - 2 * PixelNormalizedy;

			float PixelCamerax = PixelRemappedx * glm::tan(glm::radians((float)90 / 2));
			float PixelCameray = PixelRemappedy * glm::tan(glm::radians((float)90 / 2));

			glm::vec3 PcameraSpace(PixelCamerax, PixelCameray, -1);
			glm::vec3 rayDirection = glm::normalize(PcameraSpace - glm::vec3(0, 0, 0));

			std::vector<Shape*> availableShapes;
			availableShapes.push_back(RedSphere);
			availableShapes.push_back(YellowSphere);
			availableShapes.push_back(LightBlueSphere);
			availableShapes.push_back(LightGreySphere);
			availableShapes.push_back(TestPlane);
			availableShapes.push_back(TestTriangle);

			std::vector<Sphere*> availableSpheres;
			availableSpheres.push_back(RedSphere);
			availableSpheres.push_back(YellowSphere);
			availableSpheres.push_back(LightBlueSphere);
			availableSpheres.push_back(LightGreySphere);

			Sphere* closestSphere = nullptr;
			float shortestDist = 100000;

			HitInfo currentHitInfo;

			if (TestPlane->CheckIntersection(rayDirection, glm::vec3(0, 0, 0), currentHitInfo)) {
				shortestDist = TestPlane->distToCamera;
			}

			for (int i = 0; i < availableSpheres.size(); ++i) {
				if (availableSpheres[i]->CheckIntersection(rayDirection, glm::vec3(0, 0, 0), currentHitInfo)) {
					if (availableSpheres[i]->distToCamera < shortestDist) {
						shortestDist = availableSpheres[i]->distToCamera;
						closestSphere = availableSpheres[i];
					}
				}
			}

			if (TestTriangle->CheckIntersection(rayDirection, glm::vec3(0, 0, 0), currentHitInfo)) {
				ofs << (unsigned char)((TestTriangle->color.r * 255)) <<
					(unsigned char)((TestTriangle->color.g * 255)) <<
					(unsigned char)((TestTriangle->color.b * 255));
			}
			else {
				ofs << (unsigned char)(182) <<
					(unsigned char)(230) <<
					(unsigned char)(250);
			}

			/*if (closestSphere != nullptr) {
				ofs << (unsigned char)((closestSphere->color.r * 255)) <<
					(unsigned char)((closestSphere->color.g * 255)) <<
					(unsigned char)((closestSphere->color.b * 255));
			}
			else if (TestPlane->CheckIntersection(rayDirection, glm::vec3(0, 0, 0))) {
				ofs << (unsigned char)(TestPlane->color.r * 255) <<
					(unsigned char)(TestPlane->color.g * 255) <<
					(unsigned char)(TestPlane->color.b * 255);
			}
			else {
				ofs << (unsigned char)(182) <<
					(unsigned char)(230) <<
					(unsigned char)(250);
			}*/


			//else if (closestSphere != nullptr) {
			//		ofs << (unsigned char)(closestSphere->color.r * 255) <<
			//		(unsigned char)(closestSphere->color.g * 255) <<
			//		(unsigned char)(closestSphere->color.b * 255);
			//}


			/*for (int i = 0; i < availableShapes.size(); ++i) {
				if (availableShapes[i]->CheckIntersection(rayDirection, glm::vec3(0,0,0))) {
					if (availableShapes[i]->distToCamera < shortestDistToObject) {
						closestShape = availableShapes[i];
					}
				}
			}*/

			//std::vector<Sphere*> intersectedSpheres;
			//if (RedSphere->CheckIntersection(rayDirection)) {
			//	intersectedSpheres.push_back(RedSphere);
			//}
			//if (YellowSphere->CheckIntersection(rayDirection)) {
			//	intersectedSpheres.push_back(YellowSphere);
			//}
			//if (LightBlueSphere->CheckIntersection(rayDirection)) {
			//	intersectedSpheres.push_back(LightBlueSphere);
			//}
			//if (LightGreySphere->CheckIntersection(rayDirection)) {
			//	intersectedSpheres.push_back(LightGreySphere);
			//}
			//if (DarkGreySphere->CheckIntersection(rayDirection)) {
			//	intersectedSpheres.push_back(DarkGreySphere);
			//}

			//if (!intersectedSpheres.empty()) {
			//	Sphere* closestSphere = intersectedSpheres.front();
			//	float shortestDistFound = 100000;
			//	for (int i = 0; i < intersectedSpheres.size(); ++i) {
			//		if (intersectedSpheres[i]->shortestDistToCamera < shortestDistFound) {
			//			shortestDistFound = intersectedSpheres[i]->shortestDistToCamera;
			//			closestSphere = intersectedSpheres[i];
			//		}
			//	}
			//	ofs << (unsigned char)(closestSphere->color.r * 255) <<
			//		(unsigned char)(closestSphere->color.g * 255) <<
			//		(unsigned char)(closestSphere->color.b * 255);
			//}
			//else if(TestPlane->CheckIntersection(rayDirection)){
			//	ofs << (unsigned char)(40) <<
			//		(unsigned char)(40) <<
			//		(unsigned char)(40);
			//		(unsigned char)(0 * 255);
			//}	
			//else {
			//	ofs << (unsigned char)(182) <<
			//		(unsigned char)(230) <<
			//		(unsigned char)(250);
			//}
			//ofs << (unsigned char)(rayDirection.x * 255) <<
			//	(unsigned char)(rayDirection.y * 255) <<
			//	
		}
	}

	//std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	//ofs << "P6\n" << SCREEN_WIDTH << " " << SCREEN_HEIGHT << "\n255\n";
	//for (unsigned y = 0; y < SCREEN_HEIGHT; ++y)
	//{
	//	for (unsigned x = 0; x < SCREEN_WIDTH; ++x)
	//	{
	//		ofs << (unsigned char)(std::min((float)1, (float)ScreenImageBuffer[x][y].x) * 255) <<
	//			(unsigned char)(std::min((float)1, (float)ScreenImageBuffer[x][y].y) * 255) <<
	//			(unsigned char)(std::min((float)1, (float)ScreenImageBuffer[x][y].z) * 255);
	//	}
	//}
	ofs.close();

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("Cyberspace", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	if (window == nullptr) {
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		SDL_Log("Could not create a renderer: %s", SDL_GetError());
		return -1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	GLenum initState = glewInit();

	if (initState != GLEW_OK) {
		std::cout << glewGetErrorString(glewInit()) << std::endl;
	}

	while (true)
	{
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_QUIT:
				exit(0);
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					exit(0);
					break;
				}
				break;
			}
		}

		//Uint8 redVal = rand() % 255;
		//Uint8 greenVal = rand() % 255;
		//Uint8 blueVal = rand() % 255;

		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
		SDL_RenderClear(renderer);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}