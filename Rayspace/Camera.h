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
	Camera(glm::vec3 _Position, glm::vec3 _Direction, GLfloat _MovementSpeed, GLfloat _MouseSensitivity);
	~Camera();

	glm::vec3 Position;
	glm::vec3 Direction;
	glm::vec3 Front;
	glm::vec3 Up;

	glm::vec3 Right = glm::vec3(1,0,0);
	glm::vec3 WorldUp = glm::vec3(0, 1, 0);

	GLfloat Yaw;
	GLfloat Pitch;

	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;

	void Update();

	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 _ModifiedPos);

	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;

	float AspectRatio;
};

