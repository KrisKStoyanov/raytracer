#pragma once
#include "Shape.h"
#include "glm/glm.hpp"

class Box : public Shape
{
public:
	Box(glm::vec3 _PlaneMin, glm::vec3 _PlaneMax);
	~Box();
	glm::vec3 PlaneMin;
	glm::vec3 PlaneMax;
	bool CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _hitInfo);
};

