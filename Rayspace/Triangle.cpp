#include "Triangle.h"

Triangle::Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c, glm::vec3 _AmbientC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC) {
	a = _a;
	b = _b;
	c = _c;
	AmbientC = _AmbientC;
	DiffuseC = _DiffuseC;
	SpecularC = _SpecularC;
}

bool Triangle::CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo)
{
	glm::vec3 e1 = b - a;
	glm::vec3 e2 = c - a;
	float u = glm::dot((_cameraPos - a), (glm::cross(_ray, e2))) / glm::dot(e1, (glm::cross(_ray, e2)));
	float v = glm::dot(_ray,glm::cross((_cameraPos - a), e1))/glm::dot(e1, glm::cross(_ray, e2));
	float Dist = glm::dot(e2, glm::cross((_cameraPos - a), e1)) / glm::dot(e1, glm::cross(_ray, e2));

	if (u < 0 || u > 1) {
		return false;
	}
	else if (v < 0 || (u + v) > 1) {
		return false;
	}

	_hitInfo.AmbientC = AmbientC;
	_hitInfo.DiffuseC = DiffuseC;
	_hitInfo.SpecularC = SpecularC;
	_hitInfo.Distance = Dist;
	_hitInfo.IntPoint = glm::vec3(0, 0, 0);
	_hitInfo.HitStatus = true;
	return true;
}

Triangle::~Triangle()
{
}