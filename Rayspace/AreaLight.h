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
#include "Light.h"
class AreaLight : public Light 
{
public:
	AreaLight(glm::vec3 _Position, glm::vec3 _ColorInt, glm::vec3 _Size);
	~AreaLight();

	glm::vec3 MinPoint;
	glm::vec3 MaxPoint;
};

