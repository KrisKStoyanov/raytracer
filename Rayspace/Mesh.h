#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Triangle.h"
#include "Vertex.h"

class Mesh : public Shape
{
public:
	Mesh(std::vector<glm::vec3> _VertexCollection, std::vector<glm::vec3> _NormalCollection);
	~Mesh();

	bool CheckIntersection(glm::vec3 _Ray, glm::vec3 _CameraPos, HitInfo &_HitInfo);

	std::vector<Triangle*> TriangleCollection;
	std::vector<glm::vec3> NormalCollection;
};

