#pragma once
#include "Shape.h"
#include "glm/glm.hpp"
class Plane : public Shape
{
public:
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec4 color;
	Plane(glm::vec3 _pos, glm::vec3 _normal, glm::vec4 _color);
	bool CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo);
private:
	~Plane();
};

