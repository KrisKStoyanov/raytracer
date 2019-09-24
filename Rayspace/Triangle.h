#pragma once
#include "Shape.h"
#include <glm/glm.hpp>
class Triangle : public Shape
{
public:
	Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c, glm::vec4 _color);
	bool CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo);
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	glm::vec4 color;
private:
	~Triangle();
};

