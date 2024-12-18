#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Triangle.h"
#include "Box.h"

class Mesh : public Shape
{
public:
	Mesh(std::vector<glm::vec3> _VertexCollection, std::vector<glm::vec3> _NormalCollection,
		glm::vec3 _AmbientC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC, float _Shininess,
		bool _ApplyBoundingBox = true);
	Mesh();
	~Mesh();

	bool CheckIntersection(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo);

	std::vector<Triangle*> TriangleCollection;
	std::vector<glm::vec3> NormalCollection;

	Box* BoundingBox = NULL;
};

