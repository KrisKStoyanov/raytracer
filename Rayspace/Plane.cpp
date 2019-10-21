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
	//if (Denominator <= 0) {
	//	return false;
	//}

	float DistToInt = (glm::dot(Position - _RayOrigin, Normal)) / Denominator;
	if (DistToInt <= 0) {
		return false;
	}

	glm::vec3 IntPoint = _RayOrigin + _RayDirection * DistToInt;
	glm::vec3 ExitPoint = IntPoint + Normal;

	if (_HitInfo.Distance > DistToInt ||
		_HitInfo.Distance == 0.0f) {
		_HitInfo.AmbientC = AmbientC;
		_HitInfo.DiffuseC = DiffuseC;
		_HitInfo.SpecularC = SpecularC;
		_HitInfo.Shininess = Shininess;

		_HitInfo.IncidentRayDir = _RayDirection;
		_HitInfo.IncidentRayOrigin = _RayOrigin;

		_HitInfo.Distance = DistToInt;
		_HitInfo.IntPoint = IntPoint;
		_HitInfo.Normal = Normal;
		_HitInfo.ExitPoint = ExitPoint;

		_HitInfo.ReflRayDir = _RayDirection - 2 * glm::dot(glm::normalize(_RayDirection), _HitInfo.Normal) * _HitInfo.Normal;
		_HitInfo.ReflRayOrigin = _HitInfo.IntPoint + _HitInfo.Normal * 1e-4f;

		_HitInfo.Intersected = true;
	}

	return true;

}
