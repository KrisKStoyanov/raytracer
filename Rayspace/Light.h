#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

class Light
{
public:
	Light(glm::vec3 _Position, glm::vec3 _ColorInt);
	~Light();

	glm::vec3 Position;
	glm::vec3 ColorIntensity = glm::vec3(1.0f, 1.0f, 1.0f);
};

