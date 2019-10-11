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

enum Effect {
	LIGHTING,
	SHADOWS,
	REFLECTIONS
};

class Raytracer
{
public:
	Raytracer();
	~Raytracer();

	bool Init(std::string _WindowName, unsigned int _WindowWidth, unsigned int _WindowHeight, SDL_RendererFlags _RenderingFlag);
	void Start();
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

	glm::vec3 CR_AmbientColor;
	
	Light* CR_PointLight = NULL;

	std::vector<Shape*> CR_Shapes;

	SDL_Window* CR_MainWindow = NULL;
	SDL_Surface* CR_ScreenSurface = NULL;
	SDL_Surface* CR_BufferSurface = NULL;

	Camera* CR_MainCamera = NULL;

	SDL_GLContext CR_OGL_Context;
	LocalState CR_CurrentState;

	float CR_ScreenAspectRatio;

	glm::vec3 Raytrace(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _HitInfo, int _CurrentDepth, int _MaxDepth);

	bool CR_Effects_Shadows = true;
	bool CR_Effects_Soft_Shadows = true;
	bool CR_Effects_Reflections = true;

	void ToggleShadows();
	void ToggleReflections();
};



