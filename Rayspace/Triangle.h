#pragma once
#include "Shape.h"
#include <glm/glm.hpp>
class Triangle : public Shape
{
public:
	Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c,
		glm::vec3 _n0, glm::vec3 _n1, glm::vec3 _n2,
		glm::vec3 _AmbientC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC, float _Shininess);
	~Triangle();
	bool CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _hitInfo);

	float DistToCamera;

	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;

	glm::vec3 n0;
	glm::vec3 n1;
	glm::vec3 n2;

	//Surface Color:
	glm::vec3 AmbientC;
	glm::vec3 DiffuseC;
	glm::vec3 SpecularC;
	float Shininess;
};

