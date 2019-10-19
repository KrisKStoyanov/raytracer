#include "Camera.h"

Camera::Camera(glm::vec3 _Position)
{
	Position = _Position;
}

Camera::~Camera()
{
}

glm::vec3 Camera::GetPosition()
{
	return Position;
}

void Camera::SetPosition(glm::vec3 _ModifiedPos)
{
	Position = _ModifiedPos;
}
