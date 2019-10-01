#pragma once
#include "Shape.h"
#include "glm/glm.hpp"

class Box : public Shape
{
public:
	Box(glm::vec3 _MinPoint, glm::vec3 _MaxPoint);
	~Box();
	glm::vec3 MinPoint;
	glm::vec3 MaxPoint;
	bool CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _hitInfo);
};

