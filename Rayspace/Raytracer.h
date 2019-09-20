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

#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "objloader.h"
#include "Raytracer.h"
#include "Camera.h"
#include "Mesh.h"

enum LocalState {
	STARTING,
	ACTIVE,
	INACTIVE
};

class Raytracer
{
public:
	Raytracer();
	~Raytracer();

	bool Init(std::string _WindowName, unsigned int _WindowWidth, unsigned int _WindowHeight, SDL_RendererFlags _RenderingFlag);
	void Start();
	void PrintOGLVersion();
	void CheckSDLError(int line);
	void Configure(std::vector<Shape*> _Shapes);
	void Update();
	void Deactivate();

	void PrintProgramLog(GLuint _ProgramID);
	void PrintShaderLog(GLuint _ShaderID);

	unsigned int CR_WindowWidth;
	unsigned int CR_WindowHeight;

	SDL_Window* CR_MainWindow = nullptr;

	SDL_GLContext CR_OGL_Context;
	LocalState CR_CurrentState;

	float CR_ScreenAspectRatio;
};



