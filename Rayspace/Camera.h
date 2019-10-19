#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(glm::vec3 _Position);
	~Camera();

	glm::vec3 Position;

	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 _ModifiedPos);
};

