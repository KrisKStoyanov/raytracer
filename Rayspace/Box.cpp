#include "Box.h"

Box::Box(glm::vec3 _MinPoint, glm::vec3 _MaxPoint)
{
	MinPoint = _MinPoint;
	MaxPoint = _MaxPoint;
}

Box::~Box()
{
}

bool Box::CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo)
{
	float DistMinX, DistMaxX, DistMinY, DistMaxY, DistMinZ, DistMaxZ;

	if (_RayDirection.x > 0) {
		DistMinX = (MinPoint.x - _RayOrigin.x) / _RayDirection.x;
		DistMaxX = (MaxPoint.x - _RayOrigin.x) / _RayDirection.x;
	}
	else {
		DistMinX = (MaxPoint.x - _RayOrigin.x) / _RayDirection.x;
		DistMaxX = (MinPoint.x - _RayOrigin.x) / _RayDirection.x;
	}

	if (_RayDirection.y > 0) {
		DistMinY = (MinPoint.y - _RayOrigin.y) / _RayDirection.y;
		DistMaxY = (MaxPoint.y - _RayOrigin.y) / _RayDirection.y;
	}
	else {
		DistMinY = (MaxPoint.y - _RayOrigin.y) / _RayDirection.y;
		DistMaxY = (MinPoint.y - _RayOrigin.y) / _RayDirection.y;
	}

	if ((DistMinX > DistMaxY) || (DistMinY > DistMaxX)) {
		return false;
	}

	if (DistMinY > DistMinX) {
		DistMinX = DistMinY;
	}

	if (DistMaxY < DistMaxX) {
		DistMaxX = DistMaxY;
	}

	if (_RayDirection.z > 0) {
		DistMinZ = (MinPoint.z - _RayOrigin.z) / _RayDirection.z;
		DistMaxZ = (MaxPoint.z - _RayOrigin.z) / _RayDirection.z;
	}
	else {
		DistMinZ = (MaxPoint.z - _RayOrigin.z) / _RayDirection.z;
		DistMaxZ = (MinPoint.z - _RayOrigin.z) / _RayDirection.z;
	}

	if ((DistMinX > DistMaxZ) || (DistMinZ > DistMaxX)) {
		return false;
	}

	if (DistMinZ > DistMinX) {
		DistMinX = DistMinZ;
	}

	if (DistMaxZ < DistMaxX) {
		DistMaxX = DistMaxZ;
	}

	return true;
}
