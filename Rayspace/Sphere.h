#pragma once
#include <glm/glm.hpp>
#include "Shape.h"

class Sphere : public Shape
{
public:
	Sphere(glm::vec3 _pos, glm::vec4 _color, float _radius, glm::vec4 _ambient, glm::vec4 _diffuse, glm::vec4 _specular);
	~Sphere();
	bool CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo);
	glm::vec4 color;
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;
	glm::vec3 Center;
	float radius;
};

