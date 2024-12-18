#include "Raytracer.h"

Raytracer::Raytracer() : 
	m_active(false)
	, m_pointLight(NULL)
	, m_softShadowSamples(64.0f)
	, m_areaLightCellSizeX(0.0f)
	, m_areaLightCellSizeZ(0.0f)
	, m_mainWindow(NULL)
	, m_screenSurface(NULL)
	, m_mainCamera(NULL)
	, m_primitives(true)
	, m_cubeMesh(false)
	, m_teapotMesh(false)
	, m_boundingBoxes(true)
	, m_multicoreRendering(true)
	, m_totalThreadCount(1u)
	, m_screenAspectRatio(1.0f)
	, m_fovAngle(90.0f)
	, m_screenSurfaceHeightDet(1.0f)
	, m_screenSurfaceWidthDet(1.0f)
	, m_screenPixelCount(1u)
	, m_ssaaSamples(4)
	, m_gfxShadows(true)
	, m_gfxSoftShadows(true)
	, m_gfxJitteredSoftShadows(true)
	, m_gfxRecReflections(true)
	, m_gfxSupersampling(false)

{
}

Raytracer::~Raytracer()
{
}

bool Raytracer::Init(const char* _WindowName, int _WindowWidth, int _WindowHeight)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "WARNING: SDL could not initialize!" << std::endl;
		CheckSDLError(__LINE__);
		return 0;
	}

	m_mainWindow = SDL_CreateWindow(_WindowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _WindowWidth, _WindowHeight, SDL_WINDOW_RESIZABLE);
	if (m_mainWindow == nullptr) {
		std::cout << "WARNING: Window could not be created!" << std::endl;
		CheckSDLError(__LINE__);
		return 0;
	}


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	RMask = 0xff000000;
	GMask = 0x00ff0000;
	BMask = 0x0000ff00;
	AMask = 0x000000ff;
#else
	RMask = 0x000000ff;
	GMask = 0x0000ff00;
	BMask = 0x00ff0000;
	AMask = 0xff000000;
#endif
	Setup();
	return 1;
}

void Raytracer::Setup()
{
	printf("Controls:\n"
		"[W] - Move Forward\n"
		"[S] - Move Backward\n"
		"[A] - Move Left\n"
		"[D] - Move Right\n"
		"-------------------\n"
		"[LEFT] - Move Light Position Left\n"
		"[RIGHT] - Move Light Position Right\n"
		"[UP] - Move Light Position Forward\n"
		"[DOWN] - Move Light Position Backward\n"
		"-------------------\n"
		"[1] - Toggle Shadows: ON/OFF\n"
		"[2] - Toggle Shadow Type: SOFT/HARD\n"
		"[3] - Toggle Soft Shadow Sampling: UNIFORM/JITTERED\n"
		"[4] - Toggle Recursive Reflections: ON/OFF\n"
		"[5] - Toggle Supersampling: ON/OFF\n"
		"-------------------\n"
		"[M] - Toggle Multi-core Rendering: ON/OFF\n"
		"[C] - Toggle Cube Mesh: ON/OFF\n"
		"[T] - Toggle Teapot Mesh: ON/OFF\n"
		"[I] - Toggle Soft Shadow Samples: 16/36/64\n"
		"[B] - Toggle Bounding Boxes for Custom Meshes: ON/OFF\n"
		"[P] - Toggle Primitive Object Rendering: ON/OFF\n"
		"-------------------\n"
		"[G] - Randomize Sphere Positions\n"
		"[R] - Restart Scene\n"
		"[ESC] - Exit \n"
		"-------------------\n");

	m_active = true;
	ConfigScreen();
	ConfigEnv();
	ConfigLighting();
	Start();
}

void Raytracer::ConfigScreen()
{
	m_screenSurface = SDL_GetWindowSurface(m_mainWindow);
	m_screenSurfaceHeightDet = 1.f / m_screenSurface->h;
	m_screenSurfaceWidthDet = 1.f / m_screenSurface->w;
	m_screenAspectRatio = m_screenSurface->w * m_screenSurfaceHeightDet;
	m_fovAngle = glm::tan(glm::radians(90.0f) * 0.5f);
	m_totalThreadCount = std::thread::hardware_concurrency();
	m_screenPixelCount = m_screenSurface->w * m_screenSurface->h;
	m_mainCamera = new Camera(glm::vec3(1.0f, 0.0f, 5.0f));
}

void Raytracer::ConfigLighting()
{
	m_ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	m_areaLightSize = glm::vec3(9.0f, 0.1f, 9.0f);
	m_pointLight = new Light(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	if (!m_areaLights.empty()) {
		m_areaLights.clear();
	}

	float LightsPerAreaRow = glm::floor(glm::sqrt(m_softShadowSamples));
	m_areaLightCellSizeX = (m_pointLight->Position.x - m_areaLightSize.x) / LightsPerAreaRow;
	m_areaLightCellSizeZ = (m_pointLight->Position.z - m_areaLightSize.z) / LightsPerAreaRow;
	for (int z = 0; z < LightsPerAreaRow; ++z) {
		for (int x = 0; x < LightsPerAreaRow; ++x) {
			m_areaLights.push_back(glm::vec3(m_pointLight->Position.x + x * m_areaLightCellSizeX, m_pointLight->Position.y, m_pointLight->Position.z + z * m_areaLightCellSizeZ));
		}
	}
	if (m_gfxJitteredSoftShadows) {
		srand(time(NULL));
	}
}

void Raytracer::ConfigEnv()
{
	if (!m_activeObjects.empty()) {
		for (Shape* S : m_activeObjects) {
			delete S;
		}
		m_activeObjects.clear();
	}

	if (m_primitives) {
		Sphere* TempRedSphere = new Sphere(glm::vec3(0.0f, 0.0f, -20.0f), 4.0f, glm::vec3(1.0f, 0.32f, 0.36f), glm::vec3(1.0f, 0.32f, 0.36f), glm::vec3(0.8f, 0.8f, 0.8f), 128.0f);
		Sphere* TempYellowSphere = new Sphere(glm::vec3(5.0f, -1.0f, -15.0f), 2.0f, glm::vec3(0.9f, 0.76f, 0.46f), glm::vec3(0.9f, 0.76f, 0.46f), glm::vec3(0.8f, 0.8f, 0.8f), 128.0f);
		Sphere* TempLightBlueSphere = new Sphere(glm::vec3(5.0f, 0.0f, -25.0f), 3.0f, glm::vec3(0.65f, 0.77f, 0.97f), glm::vec3(0.65f, 0.77f, 0.97f), glm::vec3(0.5f, 0.5f, 0.5f), 128.0f);
		Sphere* TempLightGreySphere = new Sphere(glm::vec3(-5.5f, 0.0f, -15.0f), 3.0f, glm::vec3(0.9f, 0.9f, 0.9f), glm::vec3(0.9f, 0.9f, 0.9f), glm::vec3(0.5f, 0.5f, 0.5f), 128.0f);
		Sphere* TempGreenSphere = new Sphere(glm::vec3(-4.0f, 4.5f, -20.0f), 2.0f, glm::vec3(0.52f, 0.94f, 0.81f), glm::vec3(0.52f, 0.94f, 0.81f), glm::vec3(0.25f, 0.25f, 0.25f), 128.0f);
		m_activeObjects.push_back(TempRedSphere);
		m_activeObjects.push_back(TempYellowSphere);
		m_activeObjects.push_back(TempLightBlueSphere);
		m_activeObjects.push_back(TempLightGreySphere);
		m_activeObjects.push_back(TempGreenSphere);
	}
	Plane* TempPlane = new Plane(glm::vec3(0.0f, -4.0f, -20.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.0f, 0.0f, 0.0f), 128.0f);
	m_activeObjects.push_back(TempPlane);

	//if (m_cubeMesh) {
	//	LoadMesh("../External Resources/cube_simple.obj", glm::vec3(0.0f, 1.0f, 0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f), 128.0f, false);
	//}

	//if (m_teapotMesh) {
	//	LoadMesh("../External Resources/teapot_simple_smooth.obj", glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f), 128.0f, m_boundingBoxes);
	//}
}

glm::vec3 Raytracer::Raytrace(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, const int _CurrentDepth, int _MaxDepth)
{
	glm::vec3 HitColor = m_ambientColor;
	HitInfo Hit = Raycast(_RayOrigin, _RayDirection);

	if (Hit.Intersected) {
		glm::vec3 AmbientColor = Hit.AmbientC * m_ambientColor;
		glm::vec3 LightDirFull = m_pointLight->Position - Hit.IntPoint;
		glm::vec3 LightDir = glm::normalize(LightDirFull);
		float DiffuseScalar = glm::dot(LightDir, Hit.Normal);
		glm::vec3 DiffuseColor = Hit.DiffuseC * m_pointLight->ColorIntensity * glm::max(0.0f, DiffuseScalar);
		glm::vec3 LightReflDir = glm::normalize(glm::reflect(LightDir, Hit.Normal));
		float SpecularScalar = glm::dot(LightReflDir, Hit.IncidentRayDir);
		glm::vec3 SpecularColor = Hit.SpecularC * m_pointLight->ColorIntensity * glm::pow(glm::max(0.0f, SpecularScalar), Hit.Shininess);
		glm::vec3 ShadingColor = HitColor = AmbientColor + DiffuseColor + SpecularColor;

		if (m_gfxShadows) {
			glm::vec3 LightRayOrigin = Hit.IntPoint + Hit.Normal * 1e-4f;
			if (m_gfxSoftShadows) {
				float AreaLightInts = 0;
				for (int s = 0; s < m_areaLights.size(); ++s) {
					LightDirFull = m_areaLights[s] - Hit.IntPoint;
					if (m_gfxJitteredSoftShadows) {
						LightDirFull = glm::vec3(
							LightDirFull.x + (float)rand() / (float)RAND_MAX / (m_areaLightCellSizeX),
							LightDirFull.y,
							LightDirFull.z + (float)rand() / (float)RAND_MAX / (m_areaLightCellSizeX));
					}
					LightDir = glm::normalize(LightDirFull);
					HitInfo LightRayHit = Raycast(LightRayOrigin, LightDir);
					float LightHitDistSqrd = LightRayHit.Distance * LightRayHit.Distance;
					float LightDirFullDistSqrd = glm::dot(LightDirFull, LightDirFull);
					if (LightRayHit.Intersected && LightRayHit.Distance > 0.f && LightHitDistSqrd < LightDirFullDistSqrd) {
						AreaLightInts++;
					}
				}
				HitColor = (AreaLightInts * AmbientColor + (m_softShadowSamples - AreaLightInts) * ShadingColor) * (1.f / m_softShadowSamples);
			}
			else {
				HitInfo LightRayHit = Raycast(LightRayOrigin, LightDir);
				float LightHitDistSqrd = LightRayHit.Distance * LightRayHit.Distance;
				float LightDirFullDistSqrd = glm::dot(LightDirFull, LightDirFull);
				if (LightRayHit.Intersected && LightRayHit.Distance > 0.0f && LightHitDistSqrd < LightDirFullDistSqrd) {
					HitColor = AmbientColor;
				}
			}
		}

		if (m_gfxRecReflections && _CurrentDepth < _MaxDepth) {
			return HitColor + Hit.SpecularC * Raytrace(Hit.ReflRayOrigin, Hit.ReflRayDir, _CurrentDepth + 1, _MaxDepth);
		}
	}
	return HitColor;
}


HitInfo Raytracer::Raycast(glm::vec3 _RayOrigin, glm::vec3 _RayDirection)
{
	HitInfo Hit;
	for (int i = 0; i < m_activeObjects.size(); ++i) {
		m_activeObjects[i]->CheckIntersection(_RayOrigin, _RayDirection, Hit);
	}
	return Hit;
}

void Raytracer::Start()
{
	if (m_screenSurface != NULL) {
		auto StartTime = std::chrono::high_resolution_clock::now();
		if (m_totalThreadCount > 0 && m_multicoreRendering) {
			std::vector<std::thread> ThreadPool(m_totalThreadCount);
			SDL_Surface* BufferSurface = SDL_CreateRGBSurface(0, m_screenSurface->w, m_screenSurface->h, 32, RMask, GMask, BMask, AMask);
			uint32_t PixelsPerThread = m_screenPixelCount * (1.f / m_totalThreadCount);
			for (int i = 0; i < m_totalThreadCount; ++i) {
				uint32_t PixelIndex = i * PixelsPerThread;
				uint32_t TargetIndex = PixelIndex + PixelsPerThread;
				ThreadPool[i] = std::thread(&Raytracer::RenderSurfaceAsync, this, BufferSurface, PixelIndex, TargetIndex);
			}
			for (int i = 0; i < m_totalThreadCount; i++) {
				ThreadPool[i].join();
			}
			SDL_FreeSurface(BufferSurface);
		}
		else {
			SDL_Surface* BufferSurface = SDL_CreateRGBSurface(0, m_screenSurface->w, m_screenSurface->h, 32, RMask, GMask, BMask, AMask);
			RenderSurface(BufferSurface);
		}

		auto EndTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed_seconds = EndTime - StartTime;
		printf("\nElapsed rendering time: %f seconds.\n"
			"-------------------\n"
			"VIDEO SETTINGS:\n"
			"Hard Shadows: %s\n"
			"Soft Shadows: %s\n"
			"Soft Shadows (Jittered): %s\n"
			"Recursive Reflections: %s\n"
			"Supersampling: %s\n"
			"Multi-core Rendering: %s\n"
			"-------------------\n"
			"SCENE SETTINGS:\n"
			"Primitives: %s\n"
			"Cube Mesh: %s\n"
			"Teapot Mesh: %s\n"
			"Bounding Boxes: %s\n"
			"-------------------\n",
			elapsed_seconds.count(),
			m_gfxShadows ? "ON" : "OFF",
			m_gfxSoftShadows ? "ON" : "OFF",
			m_gfxJitteredSoftShadows ? "ON" : "OFF",
			m_gfxRecReflections ? "ON" : "OFF",
			m_gfxSupersampling ? "ON" : "OFF",
			m_multicoreRendering ? "ON" : "OFF",
			m_primitives ? "ON" : "OFF",
			m_cubeMesh ? "ON" : "OFF",
			m_teapotMesh ? "ON" : "OFF",
			m_boundingBoxes ? "ON" : "OFF");
		Update();
	}
}

bool Raytracer::RenderSurface(SDL_Surface* _TargetSurface)
{
	if (_TargetSurface != NULL) {
		uint32_t PixelCount = _TargetSurface->w * _TargetSurface->h;
		uint32_t* PixelCollection = (uint32_t*)_TargetSurface->pixels;

		if (m_gfxSupersampling) {
			srand(time(NULL));
			for (int i = 0; i < m_screenPixelCount; ++i) {
				uint32_t yIndex = glm::floor(i / _TargetSurface->w);
				uint32_t xIndex = i - yIndex * _TargetSurface->w;
				glm::vec3 IncidentRay = ComputeIncRayDir(xIndex, yIndex);
				glm::vec3 HitColor = Raytrace(m_mainCamera->Position, IncidentRay, 0, 4);
				for (int i = 1; i < m_ssaaSamples; ++i) {
					float JitterOffsetX = (float)rand() / ((float)RAND_MAX) - 0.5f;
					float JitterOffsetY = (float)rand() / ((float)RAND_MAX) - 0.5f;
					glm::vec3 IncidentRay = ComputeIncRayDir(xIndex, yIndex, JitterOffsetX, JitterOffsetY);
					HitColor += Raytrace(m_mainCamera->Position, IncidentRay, 0, 4);
				}
				HitColor *= 1.f / m_ssaaSamples;
				PixelCollection[i] =
					SDL_MapRGB(_TargetSurface->format,
						glm::clamp(HitColor.r * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.g * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.b * 255.0f, 0.0f, 255.0f));
			}
		}
		else {
			for (int i = 0; i < m_screenPixelCount; ++i) {
				uint32_t yIndex = glm::floor(i / _TargetSurface->w);
				uint32_t xIndex = i - yIndex * _TargetSurface->w;
				glm::vec3 RayDirection = ComputeIncRayDir(xIndex, yIndex);
				glm::vec3 HitColor = Raytrace(m_mainCamera->Position, RayDirection, 0, 4);
				PixelCollection[i] =
					SDL_MapRGB(_TargetSurface->format,
						glm::clamp(HitColor.r * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.g * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.b * 255.0f, 0.0f, 255.0f));
			}
		}
		SDL_Surface* OptimizedSurface = SDL_ConvertSurface(_TargetSurface, m_screenSurface->format, 0);
		SDL_BlitSurface(OptimizedSurface, NULL, m_screenSurface, NULL);
		SDL_FreeSurface(_TargetSurface);
		SDL_FreeSurface(OptimizedSurface);
		SDL_UpdateWindowSurface(m_mainWindow);

		return true;
	}
	return false;
}

void Raytracer::RenderSurfaceAsync(SDL_Surface* _TargetSurface, uint32_t _PixelIndex, uint32_t _TargetIndex)
{
	if (_PixelIndex < _TargetIndex) {
		uint32_t* PixelCollection = (uint32_t*)_TargetSurface->pixels;
		if (SDL_MUSTLOCK(_TargetSurface)) {
			SDL_LockSurface(_TargetSurface);
		}
		if (m_gfxSupersampling) {
			srand(time(NULL));
			for (_PixelIndex; _PixelIndex < _TargetIndex; ++_PixelIndex) {
				uint32_t yIndex = glm::floor(_PixelIndex / _TargetSurface->w);
				uint32_t xIndex = _PixelIndex - yIndex * _TargetSurface->w;
				glm::vec3 IncidentRay = ComputeIncRayDir(xIndex, yIndex);
				HitInfo Hit = Raycast(m_mainCamera->Position, IncidentRay);
				glm::vec3 HitColor = Raytrace(m_mainCamera->Position, IncidentRay, 0, 4);
				for (int i = 1; i < m_ssaaSamples; ++i) {
					float JitterOffsetX = (float)rand() / ((float)RAND_MAX) - 0.5f;
					float JitterOffsetY = (float)rand() / ((float)RAND_MAX) - 0.5f;
					glm::vec3 IncidentRay = ComputeIncRayDir(xIndex, yIndex, JitterOffsetX, JitterOffsetY);
					HitColor += Raytrace(m_mainCamera->Position, IncidentRay, 0, 4);
				}
				HitColor *= 1.f / m_ssaaSamples;
				PixelCollection[_PixelIndex] =
					SDL_MapRGB(_TargetSurface->format,
						glm::clamp(HitColor.r * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.g * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.b * 255.0f, 0.0f, 255.0f));
			}
		}
		else {
			for (_PixelIndex; _PixelIndex < _TargetIndex; ++_PixelIndex) {
				uint32_t yIndex = glm::floor(_PixelIndex / _TargetSurface->w);
				uint32_t xIndex = _PixelIndex - yIndex * _TargetSurface->w;
				glm::vec3 RayDirection = ComputeIncRayDir(xIndex, yIndex);
				glm::vec3 HitColor = Raytrace(m_mainCamera->Position, RayDirection, 0, 4);
				PixelCollection[_PixelIndex] =
					SDL_MapRGB(_TargetSurface->format,
						glm::clamp(HitColor.r * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.g * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.b * 255.0f, 0.0f, 255.0f));
			}
		}

		if (SDL_MUSTLOCK(_TargetSurface)) {
			SDL_UnlockSurface(_TargetSurface);
		}
		SDL_BlitSurface(_TargetSurface, NULL, m_screenSurface, NULL);
		if (m_mainWindow != nullptr) {
			SDL_UpdateWindowSurface(m_mainWindow);
		}
	}
}

glm::vec3 Raytracer::ComputeIncRayDir(int _SurfaceX, int _SurfaceY, float _OffsetX, float _OffsetY)
{
	float PixelCameraSpaceZ = m_mainCamera->Position.z - 1.0f;

	float PixelNormalizedY = (_SurfaceY + 0.5f + _OffsetY) * m_screenSurfaceHeightDet;
	float PixelRemappedY = 1.0f - 2.0f * PixelNormalizedY;

	float PixelNormalizedX = (_SurfaceX + 0.5f + _OffsetY) * m_screenSurfaceWidthDet;
	float PixelRemappedX = (2.0f * PixelNormalizedX - 1.0f) * m_screenAspectRatio;

	float PixelCameraX = PixelRemappedX * m_fovAngle;
	float PixelCameraY = PixelRemappedY * m_fovAngle;

	glm::vec3 PixelCameraSpacePos(PixelCameraX + m_mainCamera->Position.x, PixelCameraY + m_mainCamera->Position.y, PixelCameraSpaceZ);
	glm::vec3 IncidentRayDir = glm::normalize(PixelCameraSpacePos - m_mainCamera->Position);
	return IncidentRayDir;
}

void Raytracer::Update()
{
	while (m_active) {
		SDL_Event CurrentEvent;
		while (SDL_PollEvent(&CurrentEvent)) {
			if (CurrentEvent.type == SDL_QUIT) {
				m_active = false;
				break;
			}
			if (CurrentEvent.type == SDL_WINDOWEVENT) {
				switch (CurrentEvent.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					ConfigScreen();
					Start();
					break;
				default:
					break;
				}
			}
			if (CurrentEvent.type == SDL_KEYDOWN) {
				switch (CurrentEvent.key.keysym.sym) {
				case SDLK_ESCAPE:
					printf("Hotkey Pressed: [ESC]\n");
					m_active = false;
					break;
				case SDLK_w:
					m_mainCamera->SetPosition(m_mainCamera->Position + glm::vec3(0.0f, 0.0f, -1.0f));
					printf("Hotkey Pressed: [W]\n");
					Start();
					break;
				case SDLK_a:
					m_mainCamera->SetPosition(m_mainCamera->Position + glm::vec3(-1.0f, 0.0f, 0.0f));
					printf("Hotkey Pressed: [A]\n");
					Start();
					break;
				case SDLK_d:
					m_mainCamera->SetPosition(m_mainCamera->Position + glm::vec3(1.0f, 0.0f, 0.0f));
					printf("Hotkey Pressed: [D]\n");
					Start();
					break;
				case SDLK_s:
					m_mainCamera->SetPosition(m_mainCamera->Position + glm::vec3(0.0f, 0.0f, 1.0f));
					printf("Hotkey Pressed: [S]\n");
					Start();
					break;
				case SDLK_1:
					printf("Hotkey Pressed: [1]\n");
					ToggleShadows();
					break;
				case SDLK_2:
					printf("Hotkey Pressed: [2]\n");
					ToggleShadowType();
					break;
				case SDLK_3:
					printf("Hotkey Pressed: [3]\n");
					ToggleSoftShadowType();
					break;
				case SDLK_4:
					printf("Hotkey Pressed: [4]\n");
					ToggleReflections();
					break;
				case SDLK_5:
					printf("Hotkey Pressed: [5]\n");
					ToggleSupersampling();
					break;
				case SDLK_c:
					printf("Hotkey Pressed: [C]\n");
					ToggleCubeMesh();
					break;
				case SDLK_t:
					printf("Hotkey Pressed: [T]\n");
					ToggleTeapotMesh();
					break;
				case SDLK_m:
					printf("Hotkey Pressed: [M]\n");
					ToggleMulticoreRendering();
					break;
				case SDLK_UP:
					printf("Hotkey Pressed: [UP]\n");
					m_pointLight->SetPosition(m_pointLight->Position + glm::vec3(0.0f, 0.0f, -1.0f));
					ConfigLighting();
					Start();
					break;
				case SDLK_DOWN:
					printf("Hotkey Pressed: [DOWN]\n");
					m_pointLight->SetPosition(m_pointLight->Position + glm::vec3(0.0f, 0.0f, 1.0f));
					ConfigLighting();
					Start();
					break;
				case SDLK_LEFT:
					printf("Hotkey Pressed: [LEFT]\n");
					m_pointLight->SetPosition(m_pointLight->Position + glm::vec3(-1.0f, 0.0f, 0.0f));
					ConfigLighting();
					Start();
					break;
				case SDLK_RIGHT:
					printf("Hotkey Pressed: [RIGHT]\n");
					m_pointLight->SetPosition(m_pointLight->Position + glm::vec3(1.0f, 0.0f, 0.0f));
					ConfigLighting();
					Start();
					break;
				case SDLK_i:
					printf("Hotkey Pressed: [I]\n");
					ToggleSoftShadowSamples();
					ConfigLighting();
					Start();
					break;
				case SDLK_b:
					printf("Hotkey Pressed: [B]\n");
					ToggleBoundingBoxes();
					Start();
					break;
				case SDLK_p:
					printf("Hotkey Pressed: [P]\n");
					TogglePrimitives();
					Start();
					break;
				case SDLK_g:
					printf("Hotkey Pressed: [G]\n");
					RandomizeObjectPositions();
					Start();
					break;
				case SDLK_r:
					printf("Hotkey Pressed: [R]\n");
					m_primitives = true;
					m_cubeMesh = true;
					m_teapotMesh = false;
					ConfigScreen();
					ConfigEnv();
					Start();
					break;
				default:
					break;
				}
			}
		}
		SDL_UpdateWindowSurface(m_mainWindow);
	}

	if (m_screenSurface == NULL) {
		return;
	}
	Deactivate();
}

void Raytracer::ToggleMulticoreRendering()
{
	m_multicoreRendering = !m_multicoreRendering;
	Start();
}

void Raytracer::ToggleSupersampling()
{
	m_gfxSupersampling = !m_gfxSupersampling;
	Start();
}

void Raytracer::ToggleShadowType()
{
	m_gfxSoftShadows = !m_gfxSoftShadows;
	ConfigLighting();
	Start();
}

void Raytracer::ToggleSoftShadowType()
{
	m_gfxJitteredSoftShadows = !m_gfxJitteredSoftShadows;
	Start();
}

void Raytracer::ToggleReflections()
{
	m_gfxRecReflections = !m_gfxRecReflections;
	Start();
}

void Raytracer::ToggleCubeMesh()
{
	m_cubeMesh = !m_cubeMesh;
	ConfigEnv();
	Start();
}

void Raytracer::ToggleTeapotMesh()
{
	m_teapotMesh = !m_teapotMesh;
	ConfigEnv();
	Start();
}

void Raytracer::ToggleShadows()
{
	m_gfxShadows = !m_gfxShadows;
	Start();
}

void Raytracer::ToggleSoftShadowSamples()
{
	if (m_softShadowSamples == 16) {
		m_softShadowSamples = 36;
	}
	else if (m_softShadowSamples == 36) {
		m_softShadowSamples = 64;
	}
	else if (m_softShadowSamples == 64) {
		m_softShadowSamples = 16;
	}
	printf("Soft Shadow Samples %f\n", m_softShadowSamples);
}

void Raytracer::ToggleBoundingBoxes()
{
	m_boundingBoxes = !m_boundingBoxes;
	ConfigEnv();
}

void Raytracer::TogglePrimitives()
{
	m_primitives = !m_primitives;
	ConfigEnv();
}

void Raytracer::RandomizeObjectPositions()
{
	//Formula: Max + rand() / (RAND_MAX / (Min - Max + 1) + 1)
	for (int i = 0; i < m_activeObjects.size(); ++i) {
		m_activeObjects[i]->SetPosition(
			glm::vec3(
				m_activeObjects[i]->GetPosition().x + 2.f + (float)rand() / ((float)RAND_MAX / (-2.f - (2.f) + 1) + 1),
				m_activeObjects[i]->GetPosition().y + 2.f + (float)rand() / ((float)RAND_MAX / (-2.f - (2.f) + 1) + 1),
				m_activeObjects[i]->GetPosition().z + 2.f + (float)rand() / ((float)RAND_MAX / (-2.f - (2.f) + 1) + 1)));
	}
}

bool Raytracer::LoadMesh(const char* _FilePath, glm::vec3 _AmbienctC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC, float _Shininess, bool _ApplyBoundingBox)
{
	std::vector<glm::vec3> MeshVertices;
	std::vector<glm::vec3> MeshNormals;
	if (!loadOBJ(_FilePath, MeshVertices, MeshNormals)) {
		return false;
	}
	Mesh* TempMesh = new Mesh(MeshVertices, MeshNormals, _AmbienctC, _DiffuseC, _SpecularC, _Shininess, _ApplyBoundingBox);
	m_activeObjects.push_back(TempMesh);

	return true;
}

void Raytracer::Deactivate()
{
	for (Shape* S : m_activeObjects) {
		delete S;
	}

	m_pointLight = NULL;
	delete m_pointLight;
	m_mainCamera = NULL;
	delete m_mainCamera;
	SDL_FreeSurface(m_screenSurface);
	m_screenSurface = NULL;
	SDL_DestroyWindow(m_mainWindow);

	SDL_Quit();
}

void Raytracer::CheckSDLError(int _Line)
{
	std::string ErrorInfo = SDL_GetError();

	if (!ErrorInfo.empty()) {
		printf("SDL Error: %s\n", SDL_GetError());

		if (_Line != -1) {
			printf("Line: %d\n", _Line);
		}

		SDL_ClearError();
	}
}

