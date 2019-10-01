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

#include <chrono>

#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "objloader.h"
#include "Raytracer.h"
#include "Camera.h"
#include "Mesh.h"
#include "HitInfo.h"
#include "Light.h"

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
	void Configure(std::vector<Shape*> _Shapes, Camera* _MainCamera, glm::vec3 _AmbientLight, Light* _PointLight);
	void Render();
	void Update();
	void Deactivate();

	void PrintProgramLog(GLuint _ProgramID);
	void PrintShaderLog(GLuint _ShaderID);

	unsigned int CR_WindowWidth;
	unsigned int CR_WindowHeight;

	uint32_t RMask, GMask, BMask, AMask;

	glm::vec3 CR_AmbientLight;
	
	Light* CR_PointLight = NULL;

	std::vector<Shape*> CR_Shapes;

	SDL_Window* CR_MainWindow = NULL;
	SDL_Surface* CR_ScreenSurface = NULL;
	SDL_Surface* CR_BufferSurface = NULL;

	Camera* CR_MainCamera = NULL;

	SDL_GLContext CR_OGL_Context;
	LocalState CR_CurrentState;

	float CR_ScreenAspectRatio;

	bool WriteImageToFile(std::vector<Shape*> _Shapes);

	SDL_Surface* LoadSurface(std::string _ImageFilePath);

	void ApplySurfaceToScreen(SDL_Surface* _UpdateSurface, SDL_Surface* _ScreenSurface);
};



