#include "Sphere.h"
Sphere::Sphere(glm::vec3 _pos, glm::vec4 _color, float _radius, glm::vec4 _ambient, glm::vec4 _diffuse, glm::vec4 _specular) {
	Center = _pos;
	color = _color;
	radius = _radius;

	Ambient = _ambient;
	Diffuse = _diffuse;
	Specular = _specular;
}

Sphere::~Sphere() {

}

bool Sphere::CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo)
{
	glm::vec3 camToCenter = Center - _cameraPos;
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
	_hitInfo.ShapeCenter = Center;
	_hitInfo.HitStatus = true;
	//float t_hc = glm::sqrt(radius * radius - distFromCenterToRay);
	//double t0 = distFromRayToCenter - t_hc;
	//double t1 = distFromRayToCenter + t_hc;
	return true;
}

