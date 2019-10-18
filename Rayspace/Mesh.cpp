#include "Mesh.h"

Mesh::Mesh(std::vector<glm::vec3> _VertexCollection, std::vector<glm::vec3> _NormalCollection, glm::vec3 _AmbientC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC, float _Shininess)
{
	float MinX = _VertexCollection.front().x;
	float MinY = _VertexCollection.front().y;
	float MinZ = _VertexCollection.front().z;

	float MaxX = _VertexCollection.front().x;
	float MaxY = _VertexCollection.front().y;
	float MaxZ = _VertexCollection.front().z;

	glm::vec3 MaxPoint = _VertexCollection.front();
	for (size_t i = 0; i < _VertexCollection.size(); i+=3) {
		glm::vec3 a = _VertexCollection[i];
		glm::vec3 b = _VertexCollection[i + (unsigned int)1];
		glm::vec3 c = _VertexCollection[i + (unsigned int)2];

		if (_VertexCollection[i].x < MinX) {
			MinX = _VertexCollection[i].x;
		}
		if (_VertexCollection[i + (unsigned int)1].x < MinX) {
			MinX = _VertexCollection[i + (unsigned int)1].x;
		}
		if (_VertexCollection[i + (unsigned int)2].x < MinX) {
			MinX = _VertexCollection[i + (unsigned int)2].x;
		}

		if (_VertexCollection[i].y < MinY) {
			MinY = _VertexCollection[i].y;
		}
		if (_VertexCollection[i + (unsigned int)1].y < MinY) {
			MinY = _VertexCollection[i + (unsigned int)1].y;
		}
		if (_VertexCollection[i + (unsigned int)2].y < MinY) {
			MinY = _VertexCollection[i + (unsigned int)2].y;
		}

		if (_VertexCollection[i].z < MinZ) {
			MinZ = _VertexCollection[i].z;
		}
		if (_VertexCollection[i + (unsigned int)1].z < MinZ) {
			MinZ = _VertexCollection[i + (unsigned int)1].z;
		}
		if (_VertexCollection[i + (unsigned int)2].z < MinZ) {
			MinZ = _VertexCollection[i + (unsigned int)2].z;
		}

		if (_VertexCollection[i].x > MaxX) {
			MaxX = _VertexCollection[i].x;
		}
		if (_VertexCollection[i + (unsigned int)1].x > MaxX) {
			MaxX = _VertexCollection[i + (unsigned int)1].x;
		}
		if (_VertexCollection[i + (unsigned int)2].x > MaxX) {
			MaxX = _VertexCollection[i + (unsigned int)2].x;
		}

		if (_VertexCollection[i].y > MaxY) {
			MaxY = _VertexCollection[i].y;
		}
		if (_VertexCollection[i + (unsigned int)1].y > MaxY) {
			MaxY = _VertexCollection[i + (unsigned int)1].y;
		}
		if (_VertexCollection[i + (unsigned int)2].y > MaxY) {
			MaxY = _VertexCollection[i + (unsigned int)2].y;
		}

		if (_VertexCollection[i].z > MaxZ) {
			MaxZ = _VertexCollection[i].z;
		}
		if (_VertexCollection[i + (unsigned int)1].z > MaxZ) {
			MaxZ = _VertexCollection[i + (unsigned int)1].z;
		}
		if (_VertexCollection[i + (unsigned int)2].z > MaxZ) {
			MaxZ = _VertexCollection[i + (unsigned int)2].z;
		}

		glm::vec3 n0 = _NormalCollection[i];
		glm::vec3 n1 = _NormalCollection[i + (unsigned int)1];
		glm::vec3 n2 = _NormalCollection[i + (unsigned int)2];
		Triangle* MeshVert = new Triangle(
			a, b, c, 
			n0, n1, n2,
			_AmbientC, _DiffuseC, _SpecularC, _Shininess);
		TriangleCollection.push_back(MeshVert);
	}
	NormalCollection = _NormalCollection;
	BoundingBox = new Box(glm::vec3(MinX, MinY, MinZ), glm::vec3(MaxX, MaxY, MaxZ));
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

bool Mesh::CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo)
{
	if (BoundingBox->CheckIntersection(_RayOrigin, _RayDirection, _HitInfo)) {
		bool IntersectStatus = false;
		for (int i = 0; i < TriangleCollection.size(); ++i) {
			if (TriangleCollection[i]->CheckIntersection(_RayOrigin, _RayDirection, _HitInfo)) {
				IntersectStatus = true;
			};
		}
		return IntersectStatus;
	}
}
