#pragma once
#include <glm/glm.hpp>
#include "Shape.h"

class Sphere : public Shape
{
public:
	Sphere(glm::vec3 _pos, float _radius, glm::vec3 _ambientC, glm::vec3 _diffuseC, glm::vec3 _specularC);
	~Sphere();
	bool CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo);
	glm::vec3 AmbientC;
	glm::vec3 DiffuseC;
	glm::vec3 SpecularC;
	glm::vec3 Position;
	float Radius;
};

