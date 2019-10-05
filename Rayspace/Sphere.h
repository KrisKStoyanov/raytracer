#pragma once
#include <glm/glm.hpp>
#include "Shape.h"
#include "Box.h"

class Sphere : public Shape
{
public:
	glm::vec3 Position;
	float Radius;
	//Surface Color:
	glm::vec3 AmbientC;
	glm::vec3 DiffuseC;
	glm::vec3 SpecularC;
	float Shininess;
	Box* BoundingBox = NULL;

	Sphere(glm::vec3 _pos, float _radius, glm::vec3 _ambientC, glm::vec3 _diffuseC, glm::vec3 _specularC, float _shininess);
	~Sphere();
	bool CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _hitInfo);
};

