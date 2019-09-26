#pragma once
#include "Shape.h"
#include "glm/glm.hpp"
class Plane : public Shape
{
public:
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 AmbientC;
	glm::vec3 DiffuseC;
	glm::vec3 SpecularC;
	float Shininess;
	Plane(glm::vec3 _pos, glm::vec3 _normal, glm::vec3 _AmbientC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC, float _Shininess);
	bool CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo);
private:
	~Plane();
};

