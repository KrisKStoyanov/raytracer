#include "Box.h"

Box::Box(glm::vec3 _PlaneMin, glm::vec3 _PlaneMax)
{
	Type = BOX;

	PlaneMin = _PlaneMin;
	PlaneMax = _PlaneMax;
}

Box::~Box()
{
}

bool Box::CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo)
{
	float DistFront = PlaneMin.x - _RayOrigin.x / _RayDirection.x;
	float DistBack = PlaneMax.x - _RayOrigin.x / _RayDirection.x;

	glm::vec3 IntPoint = _RayOrigin + _RayDirection * DistFront;

	if (IntPoint.x > PlaneMin.x - 0.1f && IntPoint.x < PlaneMax.x + 0.1f &&
		IntPoint.y > PlaneMin.y - 0.1f && IntPoint.y < PlaneMax.y + 0.1f &&
		IntPoint.z > PlaneMin.z - 0.1f && IntPoint.z < PlaneMax.z + 0.1f) {
		return true;
	}

	IntPoint = _RayOrigin + _RayDirection * DistBack;

	if (IntPoint.x > PlaneMin.x - 0.1f && IntPoint.x < PlaneMax.x + 0.1f &&
		IntPoint.y > PlaneMin.y - 0.1f && IntPoint.y < PlaneMax.y + 0.1f &&
		IntPoint.z > PlaneMin.z - 0.1f && IntPoint.z < PlaneMax.z + 0.1f) {
		return true;
	}

	DistFront = PlaneMin.y - _RayOrigin.y / _RayDirection.y;
	DistBack = PlaneMax.y - _RayOrigin.y / _RayDirection.y;

	IntPoint = _RayOrigin + _RayDirection * DistFront;

	if (IntPoint.x > PlaneMin.x - 0.1f && IntPoint.x < PlaneMax.x + 0.1f &&
		IntPoint.y > PlaneMin.y - 0.1f && IntPoint.y < PlaneMax.y + 0.1f &&
		IntPoint.z > PlaneMin.z - 0.1f && IntPoint.z < PlaneMax.z + 0.1f) {
		return true;
	}

	IntPoint = _RayOrigin + _RayDirection * DistBack;

	if (IntPoint.x > PlaneMin.x - 0.1f && IntPoint.x < PlaneMax.x + 0.1f &&
		IntPoint.y > PlaneMin.y - 0.1f && IntPoint.y < PlaneMax.y + 0.1f &&
		IntPoint.z > PlaneMin.z - 0.1f && IntPoint.z < PlaneMax.z + 0.1f) {
		return true;
	}

	DistFront = PlaneMin.z - _RayOrigin.z / _RayDirection.z;
	DistBack = PlaneMax.z - _RayOrigin.z / _RayDirection.z;

	IntPoint = _RayOrigin + _RayDirection * DistFront;

	if (IntPoint.x > PlaneMin.x - 0.1f && IntPoint.x < PlaneMax.x + 0.001f &&
		IntPoint.y > PlaneMin.y - 0.1f && IntPoint.y < PlaneMax.y + 0.001f &&
		IntPoint.z > PlaneMin.z - 0.1f && IntPoint.z < PlaneMax.z + 0.001f) {
		return true;
	}

	IntPoint = _RayOrigin + _RayDirection * DistBack;

	if (IntPoint.x > PlaneMin.x - 0.1f && IntPoint.x < PlaneMax.x + 0.1f &&
		IntPoint.y > PlaneMin.y - 0.1f && IntPoint.y < PlaneMax.y + 0.1f &&
		IntPoint.z > PlaneMin.z - 0.1f && IntPoint.z < PlaneMax.z + 0.1f) {
		return true;
	}

	return false;
}
