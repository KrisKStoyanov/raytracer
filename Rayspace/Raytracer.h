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
	void CheckSDLError(int _Line);
	void ConfigEnv();
	void ConfigLighting();
	void ConfigScreen();
	void Start();
	bool RenderSurface(SDL_Surface* _TargetSurface);
	void RenderSurfaceAsync(SDL_Surface* _TargetSurface, uint32_t _PixelIndex, uint32_t _TargetIndex);
	glm::vec3 ComputeIncRayDir(int _SurfaceX, int _SurfaceY, float _OffsetX = .5f, float _OffsetY = .5f);
	void Update();
	void Deactivate();

	uint32_t 
		RMask = 0x000000ff,
		GMask = 0x0000ff00,
		BMask = 0x00ff0000,
		AMask = 0xff000000;

	glm::vec3 CR_AmbientColor;
	
	Light* CR_PointLight = NULL;
	float CR_SoftShadowSamples = 64;
	glm::vec3 CR_AreaLightSize;
	std::vector<glm::vec3> CR_AreaLights;
	float CR_AreaLightCellSizeX, CR_AreaLightCellSizeZ;

	std::vector<Shape*> CR_ActiveObjects;

	SDL_Window* CR_MainWindow = NULL;
	Camera* CR_MainCamera = NULL;

	glm::vec3 Raytrace(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, int _CurrentDepth = 0, int _MaxDepth = 2);

	HitInfo Raycast(glm::vec3 _RayOrigin, glm::vec3 _RayDirection);

	int CR_SSAA_Samples = 4;

	bool CR_ES_CustomMeshes = false;
	bool CR_VFX_Shadows = true;
	bool CR_VFX_SoftShadows = false;
	bool CR_VFX_JitteredSoftShadows = false;
	bool CR_VFX_RecReflections = false;
	bool CR_VFX_Supersampling = false;

	void ToggleMeshRendering();
	void ToggleShadowType();
	void ToggleSoftShadowType();
	void ToggleReflections();
	void ToggleMulticoreRendering();
	void ToggleSupersampling();

	void ToggleSoftShadowSamples();
	void RandomizeObjectPositions();

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



