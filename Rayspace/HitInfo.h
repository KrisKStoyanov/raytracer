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

class HitInfo
{
public:
	HitInfo();
	~HitInfo();
	glm::vec4 Color;
	float ShortestDistance;
	bool HitStatus = false;
};

