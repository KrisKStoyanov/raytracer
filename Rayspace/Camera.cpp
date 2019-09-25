#include "Camera.h"

Camera::Camera(glm::vec3 _Position, glm::vec3 _Direction, GLfloat _MovementSpeed, GLfloat _MouseSensitivity)
{
	Position = _Position;
	Direction = _Direction;
	MovementSpeed = _MovementSpeed;
	MouseSensitivity = _MouseSensitivity;
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

void Camera::SetPosition(glm::vec3 _ModifiedPos)
{
	Position = _ModifiedPos;
}
