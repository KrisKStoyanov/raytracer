#include "Triangle.h"

Triangle::Triangle(glm::vec3 _a, glm::vec3 _b, glm::vec3 _c, 
	glm::vec3 _n0, glm::vec3 _n1, glm::vec3 _n2,
	glm::vec3 _AmbientC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC, float _Shininess) {

	a = _a;
	b = _b;
	c = _c;

	n0 = _n0;
	n1 = _n1;
	n2 = _n2;

	AmbientC = _AmbientC;
	DiffuseC = _DiffuseC;
	SpecularC = _SpecularC;
	Shininess = _Shininess;
}

bool Triangle::CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo)
{
	glm::vec3 e1 = b - a;
	glm::vec3 e2 = c - a;
	float u = glm::dot((_RayOrigin - a), (glm::cross(_RayDirection, e2))) / glm::dot(e1, (glm::cross(_RayDirection, e2)));
	float v = glm::dot(_RayDirection,glm::cross((_RayOrigin - a), e1))/glm::dot(e1, glm::cross(_RayDirection, e2));
	float DistToInt = glm::dot(e2, glm::cross((_RayOrigin - a), e1)) / glm::dot(e1, glm::cross(_RayDirection, e2));

	if (u < 0 || u > 1) {
		return false;
	}
	else if (v < 0 || (u + v) > 1) {
		return false;
	}

	float w = 1 - u - v;

	glm::vec3 IntPoint = _RayOrigin + _RayDirection * DistToInt;
	glm::vec3 Normal = w * n0 + u * n1 + v * n2;
	Normal = glm::normalize(Normal);
	glm::vec3 ExitPoint = IntPoint + Normal;

	DistToCamera = DistToInt;

	if (_HitInfo.Distance > DistToInt ||
		_HitInfo.Distance == 0.0f) {
		_HitInfo.AmbientC = AmbientC;
		_HitInfo.DiffuseC = DiffuseC;
		_HitInfo.SpecularC = SpecularC;
		_HitInfo.Shininess = Shininess;

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

Triangle::~Triangle()
{
}