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
	Start();
}

void Raytracer::ConfigScreen()
{
	CR_ScreenSurface = SDL_GetWindowSurface(CR_MainWindow);
	CR_ScreenSurfaceHeightDet = 1.f / CR_ScreenSurface->h;
	CR_ScreenSurfaceWidthDet = 1.f / CR_ScreenSurface->w;
	CR_ScreenAspectRatio = CR_ScreenSurface->w * CR_ScreenSurfaceHeightDet;
	CR_FOV_Angle = glm::tan(glm::radians(60.0f) * 0.5f);
	CR_ScreenWidthPadding = 0;
	CR_ScreenHeigthPadding = 0;
	CR_TotalThreadCount = std::thread::hardware_concurrency();
	if (CR_TotalThreadCount > 0 && CR_Multicore_Rendering) {
		CR_ThreadedSurfaceWidth = glm::floor(CR_ScreenSurface->w / CR_TotalThreadCount);
		CR_ThreadedSurfaceHeight = glm::floor(CR_ScreenSurface->h / CR_TotalThreadCount);
		if (CR_ThreadedSurfaceWidth * CR_TotalThreadCount != CR_ScreenSurface->w) {
			CR_ScreenWidthPadding = CR_ScreenSurface->w - CR_ThreadedSurfaceWidth * CR_TotalThreadCount;
		}
		if (CR_ThreadedSurfaceHeight * CR_TotalThreadCount != CR_ScreenSurface->h) {
			CR_ScreenHeigthPadding = CR_ScreenSurface->h - CR_ThreadedSurfaceHeight * CR_TotalThreadCount;
		}
	}
}

void Raytracer::ConfigEnv()
{
	Sphere* TempRedSphere = new Sphere(glm::vec3(0.0f, 0.0f, -20.0f), 4.0f, glm::vec3(1.0f, 0.32f, 0.36f), glm::vec3(1.0f, 0.32f, 0.36f), glm::vec3(0.8f, 0.8f, 0.8f), 128.0f);
	Sphere* TempYellowSphere = new Sphere(glm::vec3(5.0f, -1.0f, -15.0f), 2.0f, glm::vec3(0.9f, 0.76f, 0.46f), glm::vec3(0.9f, 0.76f, 0.46f), glm::vec3(0.8f, 0.8f, 0.8f), 128.0f);
	Sphere* TempLightBlueSphere = new Sphere(glm::vec3(5.0f, 0.0f, -25.0f), 3.0f, glm::vec3(0.65f, 0.77f, 0.97f), glm::vec3(0.65f, 0.77f, 0.97f), glm::vec3(0.5f, 0.5f, 0.5f), 128.0f);
	Sphere* TempLightGreySphere = new Sphere(glm::vec3(-5.5f, 0.0f, -15.0f), 3.0f, glm::vec3(0.9f, 0.9f, 0.9f), glm::vec3(0.9f, 0.9f, 0.9f), glm::vec3(0.5f, 0.5f, 0.5f), 128.0f);

	Plane* TempPlane = new Plane(glm::vec3(0.0f, -4.0f, -20.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.0f, 0.0f, 0.0f), 128.0f);
	
	if (!CR_ActiveObjects.empty()) {
		for (Shape* S : CR_ActiveObjects) {
			delete S;
		}
	}
	CR_ActiveObjects.push_back(TempRedSphere);
	CR_ActiveObjects.push_back(TempYellowSphere);
	CR_ActiveObjects.push_back(TempLightBlueSphere);
	CR_ActiveObjects.push_back(TempLightGreySphere);
	CR_ActiveObjects.push_back(TempPlane);

	CR_MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
	CR_PointLight = new Light(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	CR_AmbientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	CR_AreaLightSize = glm::vec3(9.0f, 0.1f, 9.0f);
		
	float CellsPerRow = glm::floor(glm::sqrt(CR_SoftShadowSamples));
	float CellSizeX = (CR_PointLight->Position.x - CR_AreaLightSize.x) / CellsPerRow;
	float CellSizeZ = (CR_PointLight->Position.z - CR_AreaLightSize.z) / CellsPerRow;
	for (int z = 0; z < CellsPerRow; ++z) {
		for (int x = 0; x < CellsPerRow; ++x) {
			CR_AreaLights.push_back(new Light(glm::vec3(CR_PointLight->Position.x + x * CellSizeX, CR_PointLight->Position.y, CR_PointLight->Position.z + z * CellSizeZ), CR_PointLight->ColorIntensity));
		}
	}
	if (CR_Mesh_Rendering) {
		LoadMesh("../teapot_simple_smooth.obj", glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), 128.0f);
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
		glm::vec3 ShadingColor = AmbientColor + DiffuseColor + SpecularColor;

		if (CR_Effects_Hard_Shadows) {
			glm::vec3 LightRayOrigin = Hit.IntPoint + Hit.Normal * 1e-4f;
			HitInfo LightRayHit = Raycast(LightRayOrigin, LightDir);
			float LightHitDistSqrd = LightRayHit.Distance * LightRayHit.Distance;
			float LightDirFullDistSqrd = glm::dot(LightDirFull, LightDirFull);
			if (LightRayHit.Intersected && LightRayHit.Distance > 0.0f && LightHitDistSqrd < LightDirFullDistSqrd) {
				HitColor = AmbientColor;
			}
		}

		if (CR_Effects_Soft_Shadows) {
			float AreaLightInts = 0;
			glm::vec3 LightRayOrigin = Hit.IntPoint + Hit.Normal * 1e-4f;
			for (int s = 0; s < CR_AreaLights.size(); ++s) {			
				LightDirFull = CR_AreaLights[s]->Position - Hit.IntPoint;
				LightDir = glm::normalize(LightDirFull);
				HitInfo LightRayHit = Raycast(LightRayOrigin, LightDir);
				float LightHitDistSqrd = LightRayHit.Distance * LightRayHit.Distance;
				float LightDirFullDistSqrd = glm::dot(LightDirFull, LightDirFull);
				if (LightRayHit.Intersected && LightRayHit.Distance > 0 && LightHitDistSqrd < LightDirFullDistSqrd) {
					AreaLightInts++;
				}
			}
			//Samples are usable to indicate total number of Area Light objects
			HitColor = (AreaLightInts * AmbientColor + (CR_SoftShadowSamples - AreaLightInts) * ShadingColor) * (1.f / CR_SoftShadowSamples);
		}

		if (CR_Effects_Reflections && _CurrentDepth < _MaxDepth) {
			return HitColor + Hit.SpecularC * Raytrace(Hit.ReflRayOrigin, Hit.ReflRayDir, _CurrentDepth+1, _MaxDepth);
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
	auto StartTime = std::chrono::high_resolution_clock::now();
	if (CR_Multicore_Rendering) {

		std::vector<std::thread*> ThreadPool(CR_TotalThreadCount);
		std::vector<SDL_Surface*> ThreadedSurfaces(CR_TotalThreadCount);

		for (unsigned int x = 0; x < CR_TotalThreadCount; ++x) {
			for (unsigned int y = 0; y < CR_TotalThreadCount; ++y) {
				//ThreadedSurfaces[y] = *SDL_CreateRGBSurface(0, CR_ThreadedSurfaceWidth, CR_ThreadedSurfaceHeight, 32, RMask, GMask, BMask, AMask);
				ThreadedSurfaces[y] = SDL_CreateRGBSurface(0, CR_ThreadedSurfaceWidth, CR_ThreadedSurfaceHeight, 32, RMask, GMask, BMask, AMask);
				ThreadPool[y] = new std::thread(&Raytracer::RenderToSurface, this, ThreadedSurfaces[y], x * CR_ThreadedSurfaceWidth, y * CR_ThreadedSurfaceHeight);
				/*ThreadPool[y] = new std::thread(&Raytracer::RenderToSurface, this, ThreadedSurfaces[y], x * CR_ThreadedSurfaceWidth, y * CR_ThreadedSurfaceHeight);*/
			}

			for (unsigned int i = 0; i < CR_TotalThreadCount; ++i) {
				ThreadPool[i]->join();
			}
		}	

		if (CR_ScreenWidthPadding > 0) {
			SDL_Surface* WidthPaddingSurface = SDL_CreateRGBSurface(0, CR_ScreenWidthPadding * CR_TotalThreadCount, CR_ThreadedSurfaceHeight * CR_TotalThreadCount, 32, RMask, GMask, BMask, AMask);
			RenderToSurface(WidthPaddingSurface, CR_ScreenSurface->w - CR_ScreenWidthPadding, 0);
		}
		if (CR_ScreenHeigthPadding > 0) {
			SDL_Surface* HeightPaddingSurface = SDL_CreateRGBSurface(0, CR_ThreadedSurfaceWidth * CR_TotalThreadCount, CR_ScreenHeigthPadding * CR_TotalThreadCount, 32, RMask, GMask, BMask, AMask);
			RenderToSurface(HeightPaddingSurface, 0, CR_ScreenSurface->h - CR_ScreenHeigthPadding);
		}
	}
	else {
		SDL_Surface* BufferSurface = SDL_CreateRGBSurface(0, CR_ScreenSurface->w, CR_ScreenSurface->h, 32, RMask, GMask, BMask, AMask);
		RenderToSurface(BufferSurface, 0, 0);
	}

	auto EndTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed_seconds = EndTime - StartTime;
	std::cout << "Total elapsed time: " << elapsed_seconds.count() << " seconds." << std::endl;

	Update();
}

bool Raytracer::RenderToSurface(SDL_Surface* _TargetSurface, int _SurfaceRectX, int _SurfaceRectY)
{
	if (_TargetSurface != NULL) {
		auto RenderStartTime = std::chrono::high_resolution_clock::now();

		if (SDL_MUSTLOCK(_TargetSurface)) {
			SDL_LockSurface(_TargetSurface);
		}
		uint32_t* PixelAddress = (uint32_t*)_TargetSurface->pixels;
		int OffsetMod = _TargetSurface->pitch * 0.25f;
		int LineOffset;

		float PixelNormalizedY;
		float PixelRemappedY;
		float PixelCameraY;

		float PixelNormalizedX;
		float PixelRemappedX;
		float PixelCameraX;

		float PixelCameraSpaceZ = CR_MainCamera->Position.z - 1.0f;

		for (int y = 0; y < _TargetSurface->h; ++y) {

			LineOffset = y * OffsetMod;

			PixelNormalizedY = (y + _SurfaceRectY + 0.5f) * CR_ScreenSurfaceHeightDet;
			PixelRemappedY = 1.0f - 2.0f * PixelNormalizedY;
			PixelCameraY = PixelRemappedY * CR_FOV_Angle;

			for (int x = 0; x < _TargetSurface->w; ++x) {

				/*Uint8* PixelAddress = (Uint8*)ImageSurface->pixels + y * ImageSurface->pitch + x * ImageSurface->format->BytesPerPixel;*/
				/*int LineOffset = y * (ImageSurface->pitch / sizeof(uint32_t));*/

				PixelNormalizedX = (x + _SurfaceRectX + 0.5f) * CR_ScreenSurfaceWidthDet;
				PixelRemappedX = (2.0f * PixelNormalizedX - 1.0f) * CR_ScreenAspectRatio;
				PixelCameraX = PixelRemappedX * CR_FOV_Angle;

				glm::vec3 PixelCameraSpacePos(PixelCameraX + CR_MainCamera->Position.x, PixelCameraY + CR_MainCamera->Position.y, PixelCameraSpaceZ);
				glm::vec3 RayDirection = glm::normalize(PixelCameraSpacePos - CR_MainCamera->Position);

				glm::vec3 HitColor = Raytrace(CR_MainCamera->Position, RayDirection, 0, 4);
				Uint32 ColorBitValue = SDL_MapRGB(_TargetSurface->format, glm::clamp(HitColor.r * 255.0f, 0.0f, 255.0f), glm::clamp(HitColor.g * 255.0f, 0.0f, 255.0f), glm::clamp(HitColor.b * 255.0f, 0.0f, 255.0f));
				PixelAddress[LineOffset + x] = ColorBitValue;
			}
		}

		if (SDL_MUSTLOCK(_TargetSurface)) {
			SDL_UnlockSurface(_TargetSurface);
		}

		SDL_Rect SurfaceDestRect;
		SurfaceDestRect.x = _SurfaceRectX;
		SurfaceDestRect.y = _SurfaceRectY;
		
		SDL_Surface* OptimizedSurface = SDL_ConvertSurface(_TargetSurface, CR_ScreenSurface->format, 0);
		SDL_BlitSurface(_TargetSurface, NULL, CR_ScreenSurface, &SurfaceDestRect);
		SDL_FreeSurface(_TargetSurface);
		SDL_FreeSurface(OptimizedSurface);
		SDL_UpdateWindowSurface(CR_MainWindow);
		
		auto RenderEndTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed_seconds = RenderEndTime - RenderStartTime;
		std::cout << "Surface rendering time: " << elapsed_seconds.count() << " seconds." << std::endl;

		return true;
	}
	return false;
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

				case SDL_WINDOWEVENT_MAXIMIZED:
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
					std::cout << "Hotkey Pressed: [ESC]" << std::endl;
					CR_Active = false;
					break;
				case SDLK_w:
					CR_MainCamera->SetPosition(CR_MainCamera->Position + glm::vec3(0.0f, 0.0f, -1.0f));
					std::cout << "Hotkey Pressed: [W]" << std::endl;
					Start();
					break;
				case SDLK_a:
					CR_MainCamera->SetPosition(CR_MainCamera->Position + glm::vec3(-1.0f, 0.0f, 0.0f));
					std::cout << "Hotkey Pressed: [A]" << std::endl;
					Start();
					break;
				case SDLK_d:
					CR_MainCamera->SetPosition(CR_MainCamera->Position + glm::vec3(1.0f, 0.0f, 0.0f));
					std::cout << "Hotkey Pressed: [D]" << std::endl;
					Start();
					break;
				case SDLK_s:
					CR_MainCamera->SetPosition(CR_MainCamera->Position + glm::vec3(0.0f, 0.0f, 1.0f));
					std::cout << "Hotkey Pressed: [S]" << std::endl;
					Start();
					break;
				case SDLK_1:
					std::cout << "Hotkey Pressed: [1]" << std::endl;
					ToggleHardShadows();
					std::cout << "Hard Shadows: " << CR_Effects_Hard_Shadows << std::endl;
					break;
				case SDLK_2:
					std::cout << "Hotkey Pressed: [2]" << std::endl;
					ToggleSoftShadows();
					std::cout << "Soft Shadows: " << CR_Effects_Soft_Shadows << std::endl;
					break;
				case SDLK_3:
					std::cout << "Hotkey Pressed: [3]" << std::endl;
					ToggleReflections();
					std::cout << "Reflections: " << CR_Effects_Reflections << std::endl;
					break;
				case SDLK_t:
					std::cout << "Hotkey Pressed: [T]" << std::endl;
					ToggleMeshRendering();
					std::cout << "Mesh Rendering: " << CR_Mesh_Rendering << std::endl;
					break;
				case SDLK_m:
					std::cout << "Hotkey Pressed: [M]" << std::endl;
					ToggleMulticoreRendering();
					std::cout << "Multicore Rendering: " << CR_Multicore_Rendering << std::endl;
					break;
				default:
					break;
				}
			}
		}
		SDL_UpdateWindowSurface(CR_MainWindow);
	}
	Deactivate();
}

void Raytracer::ToggleHardShadows()
{
	CR_Effects_Hard_Shadows = !CR_Effects_Hard_Shadows;
	CR_Effects_Soft_Shadows = false;
	Start();
}

void Raytracer::ToggleSoftShadows()
{
	CR_Effects_Soft_Shadows = !CR_Effects_Soft_Shadows;
	CR_Effects_Hard_Shadows = false;
	Start();
}

void Raytracer::ToggleReflections()
{
	CR_Effects_Reflections = !CR_Effects_Reflections;
	Start();
}

void Raytracer::ToggleMulticoreRendering()
{
	CR_Multicore_Rendering = !CR_Multicore_Rendering;
	ConfigScreen();
	Start();
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

void Raytracer::ToggleMeshRendering()
{
	CR_Mesh_Rendering = !CR_Mesh_Rendering;
	ConfigEnv();
	Start();
	//Mesh* TempMesh = new Mesh();
	//std::vector<Shape*>::iterator startIt = CR_Shapes.begin();
	//for (; startIt != CR_Shapes.end(); ++startIt) {
	//	if (*startIt == TempMesh) {
	//		CR_InactiveObjects.push_back(*startIt);
	//		CR_Shapes.erase(startIt, CR_Shapes.end());
	//	}
	//}

	//auto e = std::find(CR_Shapes.begin(), CR_Shapes.end(), (Mesh*))
	//CR_InactiveObjects.push_back();
}

void Raytracer::Deactivate()
{
	for (Light* L : CR_AreaLights) {
		delete L;
	}

	for (Shape* S : CR_ActiveObjects) {
		delete S;
	}

	CR_PointLight = NULL;
	delete CR_PointLight;
	CR_MainCamera = NULL;
	delete CR_MainCamera;
	SDL_FreeSurface(CR_ScreenSurface);
	SDL_DestroyWindow(CR_MainWindow);

	SDL_Quit();
}

void Raytracer::CheckSDLError(int line)
{
	std::string ErrorInfo = SDL_GetError();

	if (!ErrorInfo.empty()) {
		std::cout << "SDL Error: " << ErrorInfo << std::endl;

		if (line != -1) {
			std::cout << "Line: " << line << std::endl;
		}

		SDL_ClearError();
	}
}

