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
#include "Raytracer.h"

//glm::vec3** ScreenImageBuffer;

Raytracer RTracer;

int main(int argc, char* argv[]) {
	if (!RTracer.Init("Rayspace", 1024, 768)) {
		return -1;
	}

	return 0;
}