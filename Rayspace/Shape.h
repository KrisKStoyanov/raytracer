#pragma once

#include <glm/glm.hpp>

struct HitInfo
{
	glm::vec3 color;
	float distT;
	bool hit;
};

class Shape
{
public:
	Shape();
	~Shape();
	virtual bool CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo) = 0;
	float distToCamera;
};

