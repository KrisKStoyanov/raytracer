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

bool Plane::CheckIntersection(glm::vec3 _Ray, glm::vec3 _CameraPos, HitInfo& _HitInfo)
{
	float Denominator = glm::dot(_Ray, Normal);
	if (Denominator <= 0) {
		return false;
	}
	float DistToCam = (glm::dot((Position - _CameraPos),Normal))/ Denominator;

	_HitInfo.AmbientC = AmbientC;
	_HitInfo.DiffuseC = DiffuseC;
	_HitInfo.SpecularC = SpecularC;
	_HitInfo.Distance = DistToCam;
	_HitInfo.Position = Position;
	_HitInfo.IntPoint = _CameraPos + DistToCam * _Ray;
	_HitInfo.Normal = Normal;
	_HitInfo.HitStatus = true;

	return true;

}
