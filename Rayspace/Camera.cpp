#include "Camera.h"

Camera::Camera(glm::vec3 _Position, glm::vec3 _Target, GLfloat _MovementSpeed, GLfloat _MouseSensitivity)
{
}

Camera::~Camera()
{
}

void Camera::Update()
{
}

glm::vec3 Camera::GetPosition()
{
	return Position;
}
