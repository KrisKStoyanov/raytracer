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

bool Sphere::CheckIntersection(glm::vec3 _Ray, glm::vec3 _CameraPos, HitInfo& _HitInfo)
{
	//l
	glm::vec3 RelativePosToCam = Position - _CameraPos;
	//T_ca
	float RayDirScalar = glm::dot(RelativePosToCam, _Ray);
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

	glm::vec3 IntPoint = _CameraPos + DistToEntry * _Ray;
	glm::vec3 Normal = glm::normalize(Position - IntPoint);

	glm::vec3 PrimRayDir = IntPoint + DistToEntry * _CameraPos;

	_HitInfo.AmbientC = AmbientC;
	_HitInfo.DiffuseC = DiffuseC;
	_HitInfo.SpecularC = SpecularC;
	_HitInfo.Shininess = Shininess;
	_HitInfo.Distance = glm::length(RelativePosToCam);
	_HitInfo.Position = Position;
	_HitInfo.IntPoint = IntPoint;
	_HitInfo.Normal = Normal;
	_HitInfo.PrimRayDir = PrimRayDir;
	_HitInfo.HitStatus = true;
	//float t_hc = glm::sqrt(radius * radius - distFromCenterToRay);
	//double t0 = distFromRayToCenter - t_hc;
	//double t1 = distFromRayToCenter + t_hc;
	return true;
}

