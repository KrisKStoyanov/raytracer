#pragma once
#include <glm/glm.hpp>
#include "Shape.h"

class Sphere : public Shape
{
public:
	Sphere(glm::vec3 _pos, glm::vec3 _color, float _radius);
	bool CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo);
	glm::vec3 color;
private:
	glm::vec3 center;
	float radius;
	~Sphere();
};

