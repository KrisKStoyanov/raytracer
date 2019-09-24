#pragma once
#include "HitInfo.h"
#include <glm/glm.hpp>

class Shape
{
public:
	Shape();
	~Shape();
	virtual bool CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo) = 0;
};

