#include "Sphere.h"
Sphere::Sphere(glm::vec3 _pos, float _radius, glm::vec3 _ambientC, glm::vec3 _diffuseC, glm::vec3 _specularC, float _shininess) {
	Position = _pos;
	Radius = _radius;

	AmbientC = _ambientC;
	DiffuseC = _diffuseC;
	SpecularC = _specularC;
	Shininess = _shininess;
}

Sphere::~Sphere() {

}

bool Sphere::CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo)
{
	//l
	glm::vec3 RelativePosToCam = Position - _RayOrigin;
	//T_ca
	float RayDirScalar = glm::dot(RelativePosToCam, _RayDirection);
	if (RayDirScalar < 0) {
		return false;
	}
	//s*s
	float DistToRightAngle = glm::dot(RelativePosToCam, RelativePosToCam) - (RayDirScalar * RayDirScalar);
	//s
	DistToRightAngle = glm::sqrt(DistToRightAngle);
	if (DistToRightAngle > Radius) {
		return false;
	}
	//T_hc
	float DistFromRightAngleToIntPoint = glm::sqrt((Radius * Radius) - (DistToRightAngle * DistToRightAngle));
	//T_0
	float DistToEntry = RayDirScalar - DistFromRightAngleToIntPoint;
	//T_1
	float DistToExit = RayDirScalar + DistFromRightAngleToIntPoint;

	glm::vec3 IntPoint = _RayOrigin + DistToEntry * _RayDirection;
	glm::vec3 Normal = glm::normalize(Position - IntPoint);

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

