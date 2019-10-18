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

#include "HitInfo.h"

class Camera
{
public:
	Camera(glm::vec3 _Position, glm::vec3 _Direction, float _MovementSpeed, float _MouseSensitivity);
	~Camera();

	glm::vec3 Position;
	glm::vec3 Direction;
	glm::vec3 Front;
	glm::vec3 Up;

	glm::vec3 Right = glm::vec3(1,0,0);
	glm::vec3 WorldUp = glm::vec3(0, 1, 0);

	float Yaw;
	float Pitch;

	float MovementSpeed;
	float MouseSensitivity;

	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 _ModifiedPos);

	float AspectRatio;
};

