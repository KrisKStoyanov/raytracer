#include "Mesh.h"

Mesh::Mesh(std::vector<glm::vec3> _VertexCollection, std::vector<glm::vec3> _NormalCollection, glm::vec3 _AmbientC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC, float _Shininess)
{
	Type = MESH;

	glm::vec3 PlaneMin = _VertexCollection.front();
	glm::vec3 PlaneMax = _VertexCollection.front();
	for (size_t i = 0; i < _VertexCollection.size(); i+=3) {
		glm::vec3 a = _VertexCollection[i];
		glm::vec3 b = _VertexCollection[i + (unsigned int)1];
		glm::vec3 c = _VertexCollection[i + (unsigned int)2];

		if (_VertexCollection[i].y < PlaneMin.y) {
			PlaneMin = _VertexCollection[i];
		}
		if (_VertexCollection[i + (unsigned int)1].y < PlaneMin.y) {
			PlaneMin = _VertexCollection[i + (unsigned int)1];
		}
		if (_VertexCollection[i + (unsigned int)2].y < PlaneMin.y) {
			PlaneMin = _VertexCollection[i + (unsigned int)2];
		}

		if (_VertexCollection[i].y > PlaneMax.y) {
			PlaneMax = _VertexCollection[i];
		}
		if (_VertexCollection[i + (unsigned int)1].y > PlaneMax.y) {
			PlaneMax = _VertexCollection[i + (unsigned int)1];
		}
		if (_VertexCollection[i + (unsigned int)2].y > PlaneMax.y) {
			PlaneMax = _VertexCollection[i + (unsigned int)2];
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
	BoundingBox = new Box(PlaneMin, PlaneMax);
}

Mesh::~Mesh()
{
}

bool Mesh::CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo)
{
	bool IntersectStatus = false;
	for (int i = 0; i < TriangleCollection.size(); ++i) {
		if (TriangleCollection[i]->CheckIntersection(_RayOrigin, _RayDirection, _HitInfo)) {
			IntersectStatus = true;
		};
	}
	return IntersectStatus;
}
