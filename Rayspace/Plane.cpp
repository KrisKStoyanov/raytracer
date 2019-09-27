#include "Plane.h"

Plane::Plane(glm::vec3 _pos, glm::vec3 _normal, glm::vec3 _AmbientC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC, float _Shininess)
{
	Position = _pos;
	Normal = _normal;
	AmbientC = _AmbientC;
	DiffuseC = _DiffuseC;
	SpecularC = _SpecularC;
	Shininess = _Shininess;
}

Plane::~Plane()
{

}

bool Plane::CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo)
{
	float Denominator = glm::dot(_RayDirection, Normal);
	if (Denominator <= 0) {
		return false;
	}

	float DistToEntry = (glm::dot(Position - _RayOrigin, Normal)) / Denominator;

	glm::vec3 IntPoint = _RayOrigin + DistToEntry * _RayDirection;

	if (_HitInfo.Distance > DistToEntry ||
		_HitInfo.Distance == 0.0f) {
		_HitInfo.AmbientC = AmbientC;
		_HitInfo.DiffuseC = DiffuseC;
		_HitInfo.SpecularC = SpecularC;
		_HitInfo.Shininess = Shininess;

		_HitInfo.Distance = DistToEntry;
		_HitInfo.IntPoint = IntPoint;
		_HitInfo.Normal = Normal;

		_HitInfo.HitStatus = true;
	}

	return true;

}
