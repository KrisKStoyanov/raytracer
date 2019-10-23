#include "Sphere.h"
glm::vec3 Sphere::GetPosition()
{
	return Position;
}
void Sphere::SetPosition(glm::vec3 _UpdatedPosition)
{
	Position = _UpdatedPosition;
}
Sphere::Sphere(glm::vec3 _pos, float _radius, glm::vec3 _ambientC, glm::vec3 _diffuseC, glm::vec3 _specularC, float _shininess) {
	Position = _pos;
	Radius = _radius;

	AmbientC = _ambientC;
	DiffuseC = _diffuseC;
	SpecularC = _specularC;
	Shininess = _shininess;

	BoundingBox = new Box(glm::vec3(Position.x - Radius, Position.y - Radius, Position.z - Radius), glm::vec3(Position.x + Radius, Position.y + Radius, Position.z + Radius));
}

Sphere::~Sphere() {

}

bool Sphere::CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo)
{
	glm::vec3 RelativePosToCam = Position - _RayOrigin;
	//T_ca
	float RayDirScalar = glm::dot(RelativePosToCam, _RayDirection);
	if (RayDirScalar < 0) {
		return false;
	}
	//S
	float DistToRightAngleSqrd = glm::dot(RelativePosToCam, RelativePosToCam) - (RayDirScalar * RayDirScalar);
	float RadiusSqrd = Radius * Radius;
	if (DistToRightAngleSqrd > RadiusSqrd) {
		return false;
	}
	//T_hc
	float DistFromRightAngleToIntPoint = glm::sqrt(RadiusSqrd - DistToRightAngleSqrd);
	//T_0
	float DistToEntry = RayDirScalar - DistFromRightAngleToIntPoint;
	//T_1
	float DistToExit = RayDirScalar + DistFromRightAngleToIntPoint;

	glm::vec3 IntPoint = _RayOrigin + DistToEntry * _RayDirection;
	glm::vec3 Normal = glm::normalize(IntPoint - Position);
	glm::vec3 ExitPoint = IntPoint + Normal;

	if (_HitInfo.Distance > DistToEntry ||
		_HitInfo.Distance == 0.0f) {

		_HitInfo.AmbientC = AmbientC;
		_HitInfo.DiffuseC = DiffuseC;
		_HitInfo.SpecularC = SpecularC;
		_HitInfo.Shininess = Shininess;

		_HitInfo.IncidentRayDir = _RayDirection;
		_HitInfo.IncidentRayOrigin = _RayOrigin;

		_HitInfo.Distance = DistToEntry;
		_HitInfo.IntPoint = IntPoint;
		_HitInfo.Normal = Normal;
		_HitInfo.ExitPoint = ExitPoint;

		_HitInfo.ReflRayDir = _RayDirection - 2 * glm::dot(_RayDirection, _HitInfo.Normal) * _HitInfo.Normal;
		_HitInfo.ReflRayOrigin = _HitInfo.IntPoint + _HitInfo.Normal * 1e-4f;

		_HitInfo.Intersected = true;
	}
	return true;
}

