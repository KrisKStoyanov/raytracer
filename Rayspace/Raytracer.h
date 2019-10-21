#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <SDL.h>
#include <glm/glm.hpp>

#include <chrono>
#include <thread>

#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "objloader.h"
#include "Raytracer.h"
#include "Camera.h"
#include "Mesh.h"
#include "HitInfo.h"
#include "Light.h"

class Raytracer
{
public:
	Raytracer();
	~Raytracer();

	bool CR_Active = false;

	bool Init(const char* _WindowName, int _WindowWidth, int _WindowHeight);
	void Setup();
	void CheckSDLError(int line);
	void ConfigEnv();
	void ConfigScreen();
	void Start();
	bool RenderToSurface(SDL_Surface* _TargetSurface);
	void RenderToSurfaceAsync(SDL_Surface* _TargetSurface, uint32_t _PixelIndex, unsigned int _TargetIndex);
	glm::vec3 ComputeIncRayDir(int _SurfaceX, int SurfaceY);
	void Update();
	void Deactivate();

	uint32_t RMask, GMask, BMask, AMask;

	glm::vec3 CR_AmbientColor;
	
	Light* CR_PointLight = NULL;
	float CR_SoftShadowSamples = 64;
	glm::vec3 CR_AreaLightSize;
	std::vector<glm::vec3> CR_AreaLights;

	std::vector<Shape*> CR_ActiveObjects;

	SDL_Window* CR_MainWindow = NULL;
	Camera* CR_MainCamera = NULL;

	glm::vec3 Raytrace(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, int _CurrentDepth = 0, int _MaxDepth = 2);

	HitInfo Raycast(glm::vec3 _RayOrigin, glm::vec3 _RayDirection);

	bool CR_Mesh_Rendering = false;
	bool CR_Effects_Hard_Shadows = true;
	bool CR_Effects_Soft_Shadows = true;
	bool CR_Effects_Reflections = true;

	bool CR_Rendered_Hard_Shadows = false;
	bool CR_Rendered_Soft_Shadows = false;	
	bool CR_Rendered_Reflections = false;

	void ToggleMeshRendering();
	void ToggleHardShadows();
	void ToggleSoftShadows();
	void ToggleReflections();
	void ToggleMulticoreRendering();

	SDL_Surface* CR_ScreenSurface = NULL;
	bool CR_Multicore_Rendering = true;
	unsigned int CR_TotalThreadCount;

	float CR_ScreenAspectRatio;
	float CR_FOV_Angle;

	float CR_ScreenSurfaceHeightDet;
	float CR_ScreenSurfaceWidthDet;

	unsigned int CR_ScreenPixelCount;

	bool LoadMesh(const char* _FilePath, glm::vec3 _AmbienctC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC, float _Shininess);
};



