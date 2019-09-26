#pragma once
#include "Shape.h"
#include <glm/glm.hpp>
class Triangle : public Shape
{
public:
	Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c, glm::vec3 _AmbientC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC);
	bool CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo);
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	glm::vec3 AmbientC;
	glm::vec3 DiffuseC;
	glm::vec3 SpecularC;
private:
	~Triangle();
};

