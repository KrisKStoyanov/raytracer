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

	bool m_active = false;

	uint32_t
		RMask = 0x000000ff,
		GMask = 0x0000ff00,
		BMask = 0x00ff0000,
		AMask = 0xff000000;

	glm::vec3 m_ambientColor;

	Light* m_pointLight = NULL;
	float m_softShadowSamples = 64;
	glm::vec3 m_areaLightSize;
	std::vector<glm::vec3> m_areaLights;
	float m_areaLightCellSizeX, m_areaLightCellSizeZ;

	std::vector<Shape*> m_activeObjects;

	SDL_Window* m_mainWindow = NULL;
	SDL_Surface* m_screenSurface = NULL;
	Camera* m_mainCamera = NULL;

	HitInfo Raycast(glm::vec3 _RayOrigin, glm::vec3 _RayDirection);
	glm::vec3 Raytrace(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, int _CurrentDepth = 0, int _MaxDepth = 2);

	bool m_primitives = true;

	bool m_cubeMesh = true;
	bool m_teapotMesh = false;

	bool m_boundingBoxes = true;
	bool m_multicoreRendering = true;

	unsigned int m_totalThreadCount;

	float m_screenAspectRatio;
	float m_fovAngle;

	float m_screenSurfaceHeightDet;
	float m_screenSurfaceWidthDet;

	unsigned int	m_screenPixelCount;
	int				m_ssaaSamples = 4;

	bool m_gfxShadows = true;
	bool m_gfxSoftShadows = true;
	bool m_gfxJitteredSoftShadows = true;
	bool m_gfxRecReflections = true;
	bool m_gfxSupersampling = false;

	bool Init(const char* _WindowName, int _WindowWidth, int _WindowHeight);
	void Setup();
	void CheckSDLError(int _Line);
	void ConfigEnv();
	void ConfigLighting();
	void ConfigScreen();
	void Start();
	bool RenderSurface(SDL_Surface* _TargetSurface);
	void RenderSurfaceAsync(SDL_Surface* _TargetSurface, uint32_t _PixelIndex, uint32_t _TargetIndex);
	glm::vec3 ComputeIncRayDir(int _SurfaceX, int _SurfaceY, float _OffsetX = 0.f, float _OffsetY = 0.f);
	void Update();
	void Deactivate();

	void ToggleShadows();
	void ToggleShadowType();
	void ToggleSoftShadowType();
	void ToggleReflections();
	void ToggleSupersampling();
	void ToggleSoftShadowSamples();

	void TogglePrimitives();
	void ToggleBoundingBoxes();

	void ToggleCubeMesh();
	void ToggleTeapotMesh();

	void ToggleMulticoreRendering();

	bool LoadMesh(const char* _FilePath, glm::vec3 _AmbienctC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC, float _Shininess, bool _ApplyBoundingBox);
	void RandomizeObjectPositions();
};



