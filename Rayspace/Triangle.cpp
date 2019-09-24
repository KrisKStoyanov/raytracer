#include "Triangle.h"

Triangle::Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c, glm::vec4 _color) {
	a = _a;
	b = _b;
	c = _c;
	color = _color;
}

bool Triangle::CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo)
{
	glm::vec3 e1 = b - a;
	glm::vec3 e2 = c - a;
	float u = glm::dot((_cameraPos - a), (glm::cross(_ray, e2))) / glm::dot(e1, (glm::cross(_ray, e2)));
	float v = glm::dot(_ray,glm::cross((_cameraPos - a), e1))/glm::dot(e1, glm::cross(_ray, e2));
	float dist = glm::dot(e2, glm::cross((_cameraPos - a), e1)) / glm::dot(e1, glm::cross(_ray, e2));

	if (u < 0 || u > 1) {
		return false;
	}
	else if (v < 0 || (u + v) > 1) {
		return false;
	}

	_hitInfo.Color = color;
	_hitInfo.ShortestDistance = dist;
	_hitInfo.HitStatus = true;
	return true;
}

Triangle::~Triangle()
{
}