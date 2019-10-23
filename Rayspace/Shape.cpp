#include "Shape.h"


Shape::Shape()
{
}

Shape::~Shape()
{
}

bool Shape::CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo)
{
	return false;
}

glm::vec3 Shape::GetPosition()
{
	return glm::vec3();
}

void Shape::SetPosition(glm::vec3 _UpdatedPos)
{
}

