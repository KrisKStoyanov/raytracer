#include "Sphere.h"
Sphere::Sphere(glm::vec3 _pos, glm::vec4 _color, float _radius) {
	center = _pos;
	color = _color;
	radius = _radius;
}

Sphere::~Sphere() {

}

bool Sphere::CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo)
{
	glm::vec3 camToCenter = center - _cameraPos;
	float distFromCamToCenter = glm::dot(camToCenter, glm::normalize(_ray));
	if (distFromCamToCenter < 0) {
		return false;
	}
	float distToFromCamRightAngle = glm::length(camToCenter) * glm::length(camToCenter) - distFromCamToCenter * distFromCamToCenter;
	
	if (glm::sqrt(distToFromCamRightAngle) > radius) {
		return false;
	}

	_hitInfo.Color = color;
	_hitInfo.ShortestDistance = glm::length(camToCenter);
	_hitInfo.HitStatus = true;
	//float t_hc = glm::sqrt(radius * radius - distFromCenterToRay);
	//double t0 = distFromRayToCenter - t_hc;
	//double t1 = distFromRayToCenter + t_hc;
	return true;
}

