#include "Plane.h"

Plane::Plane(glm::vec3 _pos, glm::vec3 _normal, glm::vec3 _AmbientC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC)
{
	Position = _pos;
	Normal = _normal;
	AmbientC = _AmbientC;
	DiffuseC = _DiffuseC;
	SpecularC = _SpecularC;
}

Plane::~Plane()
{

}

bool Plane::CheckIntersection(glm::vec3 _ray, glm::vec3 _cameraPos, HitInfo& _hitInfo)
{
	float Denominator = glm::dot(_ray, Normal);

	float Dist = (glm::dot((Position - _cameraPos),Normal))/ Denominator;
	if (Dist <= 0) {
		return false;
	}

	glm::vec3 RelativeDirToCam = Position - _cameraPos;
	float t = glm::dot(RelativeDirToCam, _ray);

	_hitInfo.AmbientC = AmbientC;
	_hitInfo.DiffuseC = DiffuseC;
	_hitInfo.SpecularC = SpecularC;
	_hitInfo.Distance = Dist;
	_hitInfo.Position = Position;
	_hitInfo.IntPoint = _cameraPos + Denominator * Position;
	_hitInfo.Normal = Normal;
	_hitInfo.HitStatus = true;

	return true;

}
