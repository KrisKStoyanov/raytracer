#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
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

	glm::vec3 AmbientC;
	glm::vec3 DiffuseC;
	glm::vec3 SpecularC;
	float Shininess;

	glm::vec3 Position;
	glm::vec3 IntPoint;
	glm::vec3 Normal;
	glm::vec3 ExitPoint;

	glm::vec3 ReflRayOrigin;
	glm::vec3 ReflRayDir;

	glm::vec3 IncidentRayDir;
	glm::vec3 IncidentRayOrigin;

	float Distance = 0.0f;
	bool Intersected = false;
};

