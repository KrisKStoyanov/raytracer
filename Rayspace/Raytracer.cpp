#include "Raytracer.h"

Raytracer::Raytracer()
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

	CR_MainWindow = SDL_CreateWindow(_WindowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _WindowWidth, _WindowHeight, SDL_WINDOW_RESIZABLE);
	if (CR_MainWindow == nullptr) {
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
	CR_Active = true;
	ConfigScreen();
	ConfigEnv();
	ConfigLighting();
	Start();
}

void Raytracer::ConfigScreen()
{
	CR_ScreenSurface = SDL_GetWindowSurface(CR_MainWindow);
	CR_ScreenSurfaceHeightDet = 1.f / CR_ScreenSurface->h;
	CR_ScreenSurfaceWidthDet = 1.f / CR_ScreenSurface->w;
	CR_ScreenAspectRatio = CR_ScreenSurface->w * CR_ScreenSurfaceHeightDet;
	CR_FOV_Angle = glm::tan(glm::radians(60.0f) * 0.5f);
	CR_TotalThreadCount = std::thread::hardware_concurrency();
	CR_ScreenPixelCount = CR_ScreenSurface->w * CR_ScreenSurface->h;
}

void Raytracer::ConfigLighting()
{
	CR_MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
	CR_PointLight = new Light(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	CR_AmbientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	CR_AreaLightSize = glm::vec3(9.0f, 0.1f, 9.0f);

	if (!CR_AreaLights.empty()) {
		CR_AreaLights.clear();
	}

	float LightsPerAreaRow = glm::floor(glm::sqrt(CR_SoftShadowSamples));
	CR_AreaLightCellSizeX = (CR_PointLight->Position.x - CR_AreaLightSize.x) / LightsPerAreaRow;
	CR_AreaLightCellSizeZ = (CR_PointLight->Position.z - CR_AreaLightSize.z) / LightsPerAreaRow;
	for (int z = 0; z < LightsPerAreaRow; ++z) {
		for (int x = 0; x < LightsPerAreaRow; ++x) {
			CR_AreaLights.push_back(glm::vec3(CR_PointLight->Position.x + x * CR_AreaLightCellSizeX, CR_PointLight->Position.y, CR_PointLight->Position.z + z * CR_AreaLightCellSizeZ));
		}
	}
	if (CR_VFX_JitteredSoftShadows) {
		srand(time(NULL));
	}
}

void Raytracer::ConfigEnv()
{
	Sphere* TempRedSphere = new Sphere(glm::vec3(0.0f, 0.0f, -20.0f), 4.0f, glm::vec3(1.0f, 0.32f, 0.36f), glm::vec3(1.0f, 0.32f, 0.36f), glm::vec3(0.8f, 0.8f, 0.8f), 128.0f);
	Sphere* TempYellowSphere = new Sphere(glm::vec3(5.0f, -1.0f, -15.0f), 2.0f, glm::vec3(0.9f, 0.76f, 0.46f), glm::vec3(0.9f, 0.76f, 0.46f), glm::vec3(0.8f, 0.8f, 0.8f), 128.0f);
	Sphere* TempLightBlueSphere = new Sphere(glm::vec3(5.0f, 0.0f, -25.0f), 3.0f, glm::vec3(0.65f, 0.77f, 0.97f), glm::vec3(0.65f, 0.77f, 0.97f), glm::vec3(0.5f, 0.5f, 0.5f), 128.0f);
	Sphere* TempLightGreySphere = new Sphere(glm::vec3(-5.5f, 0.0f, -15.0f), 3.0f, glm::vec3(0.9f, 0.9f, 0.9f), glm::vec3(0.9f, 0.9f, 0.9f), glm::vec3(0.5f, 0.5f, 0.5f), 128.0f);
	Sphere* TempGreenSphere = new Sphere(glm::vec3(-4.0f, 4.5f, -20.0f), 2.0f, glm::vec3(0.52f, 0.94f, 0.81f), glm::vec3(0.52f, 0.94f, 0.81f), glm::vec3(0.25f, 0.25f, 0.25f), 128.0f);

	Plane* TempPlane = new Plane(glm::vec3(0.0f, -4.0f, -20.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.0f, 0.0f, 0.0f), 128.0f);
	
	if (!CR_ActiveObjects.empty()) {
		for (Shape* S : CR_ActiveObjects) {
			delete S;
		}
		CR_ActiveObjects.clear();
	}
	CR_ActiveObjects.push_back(TempRedSphere);
	CR_ActiveObjects.push_back(TempYellowSphere);
	CR_ActiveObjects.push_back(TempLightBlueSphere);
	CR_ActiveObjects.push_back(TempLightGreySphere);
	CR_ActiveObjects.push_back(TempGreenSphere);
	CR_ActiveObjects.push_back(TempPlane);
		
	if (!CR_ES_CustomMeshes) {
		//LoadMesh("../teapot_simple_smooth.obj", glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), 128.0f);
		LoadMesh("../cube_simple.obj", glm::vec3(0.0f, 1.0f, 0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), 128.0f);
	}
}

glm::vec3 Raytracer::Raytrace(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, const int _CurrentDepth, int _MaxDepth)
{
	glm::vec3 HitColor = CR_AmbientColor;
	HitInfo Hit = Raycast(_RayOrigin, _RayDirection);

	if (Hit.Intersected) {
		glm::vec3 AmbientColor = Hit.AmbientC * CR_AmbientColor;
		glm::vec3 LightDirFull = CR_PointLight->Position - Hit.IntPoint;
		glm::vec3 LightDir = glm::normalize(LightDirFull);
		float DiffuseScalar = glm::dot(LightDir, Hit.Normal);
		glm::vec3 DiffuseColor = Hit.DiffuseC * CR_PointLight->ColorIntensity * glm::max(0.0f, DiffuseScalar);
		glm::vec3 LightReflDir = glm::normalize(glm::reflect(LightDir, Hit.Normal));
		float SpecularScalar = glm::dot(LightReflDir, Hit.IncidentRayDir);
		glm::vec3 SpecularColor = Hit.SpecularC * CR_PointLight->ColorIntensity * glm::pow(glm::max(0.0f, SpecularScalar), Hit.Shininess);
		glm::vec3 ShadingColor = HitColor = AmbientColor + DiffuseColor + SpecularColor;

		if (CR_VFX_Shadows) {
			glm::vec3 LightRayOrigin = Hit.IntPoint + Hit.Normal * 1e-4f;
			if (CR_VFX_SoftShadows) {
				float AreaLightInts = 0;
				for (int s = 0; s < CR_AreaLights.size(); ++s) {
					LightDirFull = CR_AreaLights[s] - Hit.IntPoint;
					if (CR_VFX_JitteredSoftShadows) {
						LightDirFull = glm::vec3(
							LightDirFull.x + (float)rand() / (float)RAND_MAX / (CR_AreaLightCellSizeX),
							LightDirFull.y,
							LightDirFull.z + (float)rand() / (float)RAND_MAX / (CR_AreaLightCellSizeX));
					}
					LightDir = glm::normalize(LightDirFull);
					HitInfo LightRayHit = Raycast(LightRayOrigin, LightDir);
					float LightHitDistSqrd = LightRayHit.Distance * LightRayHit.Distance;
					float LightDirFullDistSqrd = glm::dot(LightDirFull, LightDirFull);
					if (LightRayHit.Intersected && LightRayHit.Distance > 0.f && LightHitDistSqrd < LightDirFullDistSqrd) {
						AreaLightInts++;
					}
				}
				HitColor = (AreaLightInts * AmbientColor + (CR_SoftShadowSamples - AreaLightInts) * ShadingColor) * (1.f / CR_SoftShadowSamples);
			}
			else {
				HitInfo LightRayHit = Raycast(LightRayOrigin, LightDir);
				float LightHitDistSqrd = LightRayHit.Distance * LightRayHit.Distance;
				float LightDirFullDistSqrd = glm::dot(LightDirFull, LightDirFull);
				if (LightRayHit.Intersected && LightRayHit.Distance > 0.0f && LightRayHit.Distance != Hit.Distance && LightHitDistSqrd < LightDirFullDistSqrd) {
					HitColor = AmbientColor;
				}
			}	
		}	

		if (CR_VFX_RecReflections && _CurrentDepth < _MaxDepth) {
			return HitColor + Hit.SpecularC * Raytrace(Hit.ReflRayOrigin, Hit.ReflRayDir, _CurrentDepth + 1, _MaxDepth);
		}
	}
	return HitColor;
}


HitInfo Raytracer::Raycast(glm::vec3 _RayOrigin, glm::vec3 _RayDirection)
{
	HitInfo Hit;
	for (int i = 0; i < CR_ActiveObjects.size(); ++i) {
		CR_ActiveObjects[i]->CheckIntersection(_RayOrigin, _RayDirection, Hit);
	}
	return Hit;
}

void Raytracer::Start()
{
	if (CR_ScreenSurface != NULL) {
		auto StartTime = std::chrono::high_resolution_clock::now();
		if (CR_TotalThreadCount > 0 && CR_Multicore_Rendering) {
			std::vector<std::thread> ThreadPool(CR_TotalThreadCount);
			SDL_Surface* BufferSurface = SDL_CreateRGBSurface(0, CR_ScreenSurface->w, CR_ScreenSurface->h, 32, RMask, GMask, BMask, AMask);
			uint32_t PixelsPerThread = CR_ScreenPixelCount * (1.f / CR_TotalThreadCount);
			for (int i = 0; i < CR_TotalThreadCount; ++i) {
				uint32_t PixelIndex = i * PixelsPerThread;
				uint32_t TargetIndex = PixelIndex + PixelsPerThread;
				ThreadPool[i] = std::thread(&Raytracer::RenderSurfaceAsync, this, BufferSurface, PixelIndex, TargetIndex);
			}
			for (int i = 0; i < CR_TotalThreadCount; i++) {
				ThreadPool[i].join();
			}
			SDL_FreeSurface(BufferSurface);
		}
		else {
			SDL_Surface* BufferSurface = SDL_CreateRGBSurface(0, CR_ScreenSurface->w, CR_ScreenSurface->h, 32, RMask, GMask, BMask, AMask);
			RenderSurface(BufferSurface);
		}

		auto EndTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed_seconds = EndTime - StartTime;
		std::cout << "Elapsed rendering time: " << elapsed_seconds.count() << " seconds." << std::endl;

		Update();
	}
}

bool Raytracer::RenderSurface(SDL_Surface* _TargetSurface)
{
	if (_TargetSurface != NULL) {
		uint32_t PixelCount = _TargetSurface->w * _TargetSurface->h;
		uint32_t* PixelCollection = (uint32_t*)_TargetSurface->pixels;

		if (CR_VFX_Supersampling) {
			srand(time(NULL));
			for (int i = 0; i < CR_ScreenPixelCount; ++i) {
				uint32_t yIndex = glm::floor(i / _TargetSurface->w);
				uint32_t xIndex = i - yIndex * _TargetSurface->w;
				glm::vec3 IncidentRay = ComputeIncRayDir(xIndex, yIndex);
				glm::vec3 HitColor = Raytrace(CR_MainCamera->Position, IncidentRay, 0, 4);
				for (int i = 1; i < CR_SSAA_Samples; ++i) {
					float JitterOffsetX = (float)rand() / ((float)RAND_MAX) - 0.5f;
					float JitterOffsetY = (float)rand() / ((float)RAND_MAX) - 0.5f;
					glm::vec3 IncidentRay = ComputeIncRayDir(xIndex, yIndex, JitterOffsetX, JitterOffsetY);
					HitColor += Raytrace(CR_MainCamera->Position, IncidentRay, 0, 4);
				}
				HitColor *= 1.f / CR_SSAA_Samples;
				PixelCollection[i] =
					SDL_MapRGB(_TargetSurface->format,
						glm::clamp(HitColor.r * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.g * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.b * 255.0f, 0.0f, 255.0f));
			}
		}
		else {
			for (int i = 0; i < CR_ScreenPixelCount; ++i) {
				uint32_t yIndex = glm::floor(i / _TargetSurface->w);
				uint32_t xIndex = i - yIndex * _TargetSurface->w;
				glm::vec3 RayDirection = ComputeIncRayDir(xIndex, yIndex);
				glm::vec3 HitColor = Raytrace(CR_MainCamera->Position, RayDirection, 0, 4);
				PixelCollection[i] =
					SDL_MapRGB(_TargetSurface->format,
						glm::clamp(HitColor.r * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.g * 255.0f, 0.0f, 255.0f),
						glm::clamp(HitColor.b * 255.0f, 0.0f, 255.0f));
			}
		}
		SDL_Surface* OptimizedSurface = SDL_ConvertSurface(_TargetSurface, CR_ScreenSurface->format, 0);
		SDL_BlitSurface(OptimizedSurface, NULL, CR_ScreenSurface, NULL);
		SDL_FreeSurface(_TargetSurface);
		SDL_FreeSurface(OptimizedSurface);
		SDL_UpdateWindowSurface(CR_MainWindow);

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
		if (CR_VFX_Supersampling) {
			srand(time(NULL));
			for (_PixelIndex; _PixelIndex < _TargetIndex; ++_PixelIndex) {
				uint32_t yIndex = glm::floor(_PixelIndex / _TargetSurface->w);
				uint32_t xIndex = _PixelIndex - yIndex * _TargetSurface->w;
				glm::vec3 IncidentRay = ComputeIncRayDir(xIndex, yIndex);
				HitInfo Hit = Raycast(CR_MainCamera->Position, IncidentRay);
				glm::vec3 HitColor = Raytrace(CR_MainCamera->Position, IncidentRay, 0, 4);
				for (int i = 1; i < CR_SSAA_Samples; ++i) {
					float JitterOffsetX = (float)rand() / ((float)RAND_MAX) - 0.5f;
					float JitterOffsetY = (float)rand() / ((float)RAND_MAX) - 0.5f;
					glm::vec3 IncidentRay = ComputeIncRayDir(xIndex, yIndex, JitterOffsetX, JitterOffsetY);
					HitColor += Raytrace(CR_MainCamera->Position, IncidentRay, 0, 4);
				}
				HitColor *= 1.f / CR_SSAA_Samples;
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
				glm::vec3 HitColor = Raytrace(CR_MainCamera->Position, RayDirection, 0, 4);
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
		SDL_BlitSurface(_TargetSurface, NULL, CR_ScreenSurface, NULL);
		if (this->CR_MainWindow != nullptr) {
			SDL_UpdateWindowSurface(CR_MainWindow);
		}
	}
}

glm::vec3 Raytracer::ComputeIncRayDir(int _SurfaceX, int _SurfaceY, float _OffsetX, float _OffsetY)
{
	float PixelCameraSpaceZ = CR_MainCamera->Position.z - 1.0f;

	float PixelNormalizedY = (_SurfaceY + _OffsetY) * CR_ScreenSurfaceHeightDet;
	float PixelRemappedY = 1.0f - 2.0f * PixelNormalizedY;

	float PixelNormalizedX = (_SurfaceX + _OffsetX) * CR_ScreenSurfaceWidthDet;
	float PixelRemappedX = (2.0f * PixelNormalizedX - 1.0f) * CR_ScreenAspectRatio;

	float PixelCameraX = PixelRemappedX * CR_FOV_Angle;
	float PixelCameraY = PixelRemappedY * CR_FOV_Angle;

	glm::vec3 PixelCameraSpacePos(PixelCameraX + CR_MainCamera->Position.x, PixelCameraY + CR_MainCamera->Position.y, PixelCameraSpaceZ);
	glm::vec3 IncidentRayDir = glm::normalize(PixelCameraSpacePos - CR_MainCamera->Position);
	return IncidentRayDir;
}

void Raytracer::Update()
{
	while (CR_Active) {
		SDL_Event CurrentEvent;
		while (SDL_PollEvent(&CurrentEvent)) {
			if (CurrentEvent.type == SDL_QUIT) {
				CR_Active = false;
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
					CR_Active = false;
					break;
				case SDLK_w:
					CR_MainCamera->SetPosition(CR_MainCamera->Position + glm::vec3(0.0f, 0.0f, -1.0f));
					printf("Hotkey Pressed: [W]\n");
					Start();
					break;
				case SDLK_a:
					CR_MainCamera->SetPosition(CR_MainCamera->Position + glm::vec3(-1.0f, 0.0f, 0.0f));
					printf("Hotkey Pressed: [A]\n");
					Start();
					break;
				case SDLK_d:
					CR_MainCamera->SetPosition(CR_MainCamera->Position + glm::vec3(1.0f, 0.0f, 0.0f));
					printf("Hotkey Pressed: [D]\n");
					Start();
					break;
				case SDLK_s:
					CR_MainCamera->SetPosition(CR_MainCamera->Position + glm::vec3(0.0f, 0.0f, 1.0f));
					printf("Hotkey Pressed: [S]\n");
					Start();
					break;
				case SDLK_1:
					printf("Hotkey Pressed: [1]\n");
					ToggleShadowType();
					break;
				case SDLK_2:
					printf("Hotkey Pressed: [2]\n");
					ToggleSoftShadowType();
					break;
				case SDLK_3:
					printf("Hotkey Pressed: [3]\n");
					ToggleReflections();

					break;
				case SDLK_4:
					printf("Hotkey Pressed: [4]\n");
					ToggleSupersampling();
					break;
				case SDLK_t:
					printf("Hotkey Pressed: [T]\n");
					ToggleMeshRendering();
					break;
				case SDLK_m:
					printf("Hotkey Pressed: [M]\n");
					ToggleMulticoreRendering();
					break;
				case SDLK_UP:
					printf("Hotkey Pressed: [UP]\n");
					CR_PointLight->SetPosition(CR_PointLight->Position + glm::vec3(0.0f, 0.0f, -1.0f));
					ConfigLighting();
					Start();
					break;
				case SDLK_DOWN:
					printf("Hotkey Pressed: [DOWN]\n");
					CR_PointLight->SetPosition(CR_PointLight->Position + glm::vec3(0.0f, 0.0f, 1.0f));
					ConfigLighting();
					Start();
					break;
				case SDLK_LEFT:
					printf("Hotkey Pressed: [LEFT]\n");
					CR_PointLight->SetPosition(CR_PointLight->Position + glm::vec3(-1.0f, 0.0f, 0.0f));
					ConfigLighting();
					Start();
					break;
				case SDLK_RIGHT:
					printf("Hotkey Pressed: [RIGHT]\n");
					CR_PointLight->SetPosition(CR_PointLight->Position + glm::vec3(1.0f, 0.0f, 0.0f));
					ConfigLighting();
					Start();
					break;
				case SDLK_i:
					printf("Hotkey Pressed: [I]\n");
					ToggleSoftShadowSamples();
					ConfigLighting();
					Start();
					break;
				case SDLK_g:
					printf("Hotkey Pressed: [G]\n");
					RandomizeObjectPositions();
					Start();
					break;
				case SDLK_r:
					printf("Hotkey Pressed: [R]\n");
					ConfigEnv();
					Start();
					break;
				default:
					break;
				}
			}
		}
		SDL_UpdateWindowSurface(CR_MainWindow);
	}

	if (CR_ScreenSurface == NULL) {
		return;
	}
	Deactivate();
}

void Raytracer::ToggleMulticoreRendering()
{
	CR_Multicore_Rendering = !CR_Multicore_Rendering;
	ConfigScreen();
	printf("Multicore rendering toggled\n");
	Start();
}

void Raytracer::ToggleSupersampling()
{
	CR_VFX_Supersampling = !CR_VFX_Supersampling;
	printf("Supersampling toggled\n");
	Start();
}

void Raytracer::ToggleShadowType()
{
	CR_VFX_SoftShadows = !CR_VFX_SoftShadows;
	ConfigLighting();
	printf("Shadows type toggled\n");
	Start();
}

void Raytracer::ToggleSoftShadowType()
{
	CR_VFX_JitteredSoftShadows = !CR_VFX_JitteredSoftShadows;

	printf("Soft Shadow type toggled\n");
	Start();
}

void Raytracer::ToggleReflections()
{
	CR_VFX_RecReflections = !CR_VFX_RecReflections;
	printf("Reflections toggled\n");
	Start();
}

void Raytracer::ToggleMeshRendering()
{
	CR_ES_CustomMeshes = !CR_ES_CustomMeshes;
	ConfigEnv();
	printf("Mesh rendering toggled\n");
	Start();
}

void Raytracer::ToggleSoftShadowSamples()
{
	if (CR_SoftShadowSamples == 16) {
		CR_SoftShadowSamples = 36;
	}
	else if (CR_SoftShadowSamples == 36) {
		CR_SoftShadowSamples = 64;
	}
	else if (CR_SoftShadowSamples == 64) {
		CR_SoftShadowSamples = 16;
	}
	printf("Soft shadow samples %f\n", CR_SoftShadowSamples);
}

void Raytracer::RandomizeObjectPositions()
{
	//Formula: Max + rand() / (RAND_MAX / (Min - Max + 1) + 1)
	for (int i = 0; i < CR_ActiveObjects.size(); ++i) {
		CR_ActiveObjects[i]->SetPosition(
			glm::vec3(
				CR_ActiveObjects[i]->GetPosition().x + 2.f + (float)rand() / ((float)RAND_MAX / (-2.f - (2.f) + 1) + 1),
				CR_ActiveObjects[i]->GetPosition().y + 2.f + (float)rand() / ((float)RAND_MAX / (-2.f - (2.f) + 1) + 1),
				CR_ActiveObjects[i]->GetPosition().z + 2.f + (float)rand() / ((float)RAND_MAX / (-2.f - (2.f) + 1) + 1)));
	}
}

bool Raytracer::LoadMesh(const char* _FilePath, glm::vec3 _AmbienctC, glm::vec3 _DiffuseC, glm::vec3 _SpecularC, float _Shininess)
{
	std::vector<glm::vec3> MeshVertices;
	std::vector<glm::vec3> MeshNormals;
	if (!loadOBJ(_FilePath, MeshVertices, MeshNormals)) {
		return false;
	}
	Mesh* TempMesh = new Mesh(MeshVertices, MeshNormals, _AmbienctC, _DiffuseC, _SpecularC, _Shininess);
	CR_ActiveObjects.push_back(TempMesh);
	
	return true;
}

void Raytracer::Deactivate()
{
	for (Shape* S : CR_ActiveObjects) {
		delete S;
	}

	CR_PointLight = NULL;
	delete CR_PointLight;
	CR_MainCamera = NULL;
	delete CR_MainCamera;
	SDL_FreeSurface(CR_ScreenSurface);
	CR_ScreenSurface = NULL;
	SDL_DestroyWindow(CR_MainWindow);

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

