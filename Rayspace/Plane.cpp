#include "Plane.h"

Plane::Plane(glm::vec3 _pos, glm::vec3 _normal, glm::vec4 _color)
{
	pos = _pos;
	normal = _normal;
	color = _color;
}

Plane::~Plane()
{

}

bool Plane::CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo)
{
	float denominator = glm::dot(_ray, normal);

	float dist = (glm::dot((pos - _cameraPos),normal))/denominator;
	if (dist <= 0) {
		return false;
	}

	_hitInfo.Color = color;
	_hitInfo.ShortestDistance = dist;
	_hitInfo.HitStatus = true;

	return true;

}
