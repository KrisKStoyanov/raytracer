#include "Mesh.h"

Mesh::Mesh(std::vector<glm::vec3> _VertexCollection, std::vector<glm::vec3> _NormalCollection)
{
	for (size_t i = 0; i < _VertexCollection.size(); i+=3) {
		glm::vec3 a = _VertexCollection[i];
		glm::vec3 b = _VertexCollection[i + (size_t)1];
		glm::vec3 c = _VertexCollection[i + (unsigned int)2];
		glm::vec3 n0 = _NormalCollection[i];
		glm::vec3 n1 = _NormalCollection[i + (unsigned int)1];
		glm::vec3 n2 = _NormalCollection[i + (unsigned int)2];
		Triangle* MeshVert = new Triangle(
			a, b, c, 
			n0, n1, n2,
			glm::vec3(1, 1, 0), glm::vec3(1, 1, 0), glm::vec3(1, 1, 0), 128);
		TriangleCollection.push_back(MeshVert);
		delete MeshVert;
	}
	NormalCollection = _NormalCollection;
}

Mesh::~Mesh()
{
}

bool Mesh::CheckIntersection(glm::vec3 _Ray, glm::vec3 _CameraPos, HitInfo& _HitInfo)
{
	for (int i = 0; i < TriangleCollection.size(); ++i) {
		if (TriangleCollection[i]->CheckIntersection(_Ray, _CameraPos, _HitInfo)) {

			_HitInfo.AmbientC = TriangleCollection[i]->AmbientC;
			_HitInfo.DiffuseC = TriangleCollection[i]->DiffuseC;
			_HitInfo.SpecularC = TriangleCollection[i]->SpecularC;

			return true;
		}
	}
}
