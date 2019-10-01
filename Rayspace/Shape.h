#pragma once
#include "HitInfo.h"
#include <glm/glm.hpp>


class Shape
{
public:
	Shape();
	~Shape();
	virtual bool CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo) = 0;
};

