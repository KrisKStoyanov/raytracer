#include "Raytracer.h"

Raytracer::Raytracer()
{
}

Raytracer::~Raytracer()
{
}

bool Raytracer::Init(std::string _WindowName, unsigned int _WindowWidth, unsigned int _WindowHeight, SDL_RendererFlags _RenderingFlag)
{
	LocalState CurrentState = STARTING;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "WARNING: SDL could not initialize!" << std::endl;
		CheckSDLError(__LINE__);
		return 0;
	}

	CR_WindowWidth = _WindowWidth;
	CR_WindowHeight = _WindowHeight;
	CR_ScreenAspectRatio = CR_WindowWidth / CR_WindowHeight;

	CR_MainWindow = SDL_CreateWindow(_WindowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CR_WindowWidth, CR_WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (CR_MainWindow == nullptr) {
		std::cout << "WARNING: Window could not be created!" << std::endl;
		CheckSDLError(__LINE__);
		return 0;
	}

	CR_ScreenSurface = SDL_GetWindowSurface(CR_MainWindow);

	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//CR_OGL_Context = SDL_GL_CreateContext(CR_MainWindow);

	//glewExperimental = GL_TRUE;
	//GLenum initState = glewInit();

	//if (initState != GLEW_OK) {
	//	std::cout << glewGetErrorString(glewInit()) << std::endl;
	//	return 0;
	//}

	// This makes our buffer swap syncronized with the monitor's vertical refresh - Vsync
	//if (SDL_GL_SetSwapInterval(1) < 0) {
	//	std::cout << "WARNING: Vsync could not be set!" << std::endl;
	//	CheckSDLError(__LINE__);
	//}

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

	//glClearColor(0, 0, 0, 1);
	//glClear(GL_COLOR_BUFFER_BIT);
	//SDL_GL_SwapWindow(CR_MainWindow);

	//PrintOGLVersion();

	return 1;
}

void Raytracer::Start()
{
	CR_CurrentState = ACTIVE;
	Update();
}

void Raytracer::PrintOGLVersion()
{
	int value = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
	std::cout << "Powered by OpenGL " << value;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
	std::cout << "." << value << " and SDL2" << std::endl;
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

bool Raytracer::WriteImageToFile(std::vector<Shape*> _Shapes) {

	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << CR_WindowWidth << " " << CR_WindowHeight << "\n255\n";
	for (int y = 0; y < CR_WindowHeight; ++y) {
		for (int x = 0; x < CR_WindowWidth; ++x) {

			float PixelNormalizedx = (x + 0.5) / CR_WindowWidth;
			float PixelNormalizedy = (y + 0.5) / CR_WindowHeight;

			float PixelRemappedx = 2 * PixelNormalizedx - 1;
			float PixelRemappedy = 1 - 2 * PixelNormalizedy;

			PixelRemappedx = (2 * PixelNormalizedx - 1) * CR_ScreenAspectRatio;
			PixelRemappedy = 1 - 2 * PixelNormalizedy;

			float PixelCamerax = PixelRemappedx * glm::tan(glm::radians((float)90 / 2));
			float PixelCameray = PixelRemappedy * glm::tan(glm::radians((float)90 / 2));

			glm::vec3 PcameraSpace(PixelCamerax, PixelCameray, -1);
			glm::vec3 RayDirection = glm::normalize(PcameraSpace - glm::vec3(0, 0, 0));

			float DistCheck = 100000;
			glm::vec3 AmbientC;
			Uint32 ColorBitValue = 0;
			HitInfo hitInfo;

			for (int i = 0; i < _Shapes.size(); ++i) {
				if (_Shapes[i]->CheckIntersection(RayDirection, glm::vec3(0, 0, 0), hitInfo)) {
					if (hitInfo.Distance < DistCheck) {
						DistCheck = hitInfo.Distance;
						AmbientC = (glm::vec3)hitInfo.AmbientC;
						hitInfo.HitStatus = true;
					}
				}
			}
			if (!hitInfo.HitStatus) {
				ofs << (unsigned char)(182) <<
					(unsigned char)(230) <<
					(unsigned char)(250);
			}
			else {
				ofs << (unsigned char)((AmbientC.r * 255) * CR_AmbientColor.r) <<
					(unsigned char)((AmbientC.g * 255) * CR_AmbientColor.g) <<
					(unsigned char)((AmbientC.b * 255) * CR_AmbientColor.b);
			}
		}
	}
	ofs.close();

	return true;
}

void Raytracer::Configure(std::vector<Shape*> _Shapes, Camera* _MainCamera, glm::vec3 _AmbientLight, Light* _PointLight, AreaLight* _AreaLight)
{
	CR_Shapes = _Shapes;
	CR_MainCamera = _MainCamera;
	CR_AmbientColor = _AmbientLight;
	CR_PointLight = _PointLight;
	CR_AreaLight = _AreaLight;

	Render();
}

glm::vec3 Raytracer::Raytrace(glm::vec3 _RayOrigin, glm::vec3 _RayDirection, HitInfo& _Hit, int _CurrentDepth, int _MaxDepth)
{
	glm::vec3 CombinedColor = CR_AmbientColor;
	for (int i = 0; i < CR_Shapes.size(); ++i) {
		CR_Shapes[i]->CheckIntersection(_RayOrigin, _RayDirection, _Hit);
	}

	if (_Hit.HitStatus) {
		glm::vec3 AmbientColor = _Hit.AmbientC * CR_AmbientColor;
		glm::vec3 LightDirFull = CR_PointLight->Position - _Hit.IntPoint;
		glm::vec3 LightDir = glm::normalize(LightDirFull);

		float DiffuseScalar = glm::dot(LightDir, _Hit.Normal);
		glm::vec3 DiffuseColor = _Hit.DiffuseC * CR_PointLight->ColorIntensity * glm::max(0.0f, DiffuseScalar);
		glm::vec3 LightReflDir = glm::normalize(2 * -DiffuseScalar * _Hit.Normal + LightDir);
		float SpecularScalar = glm::dot(LightReflDir, _RayDirection);
		glm::vec3 SpecularColor = _Hit.SpecularC * CR_PointLight->ColorIntensity * glm::pow(glm::max(0.0f, SpecularScalar), _Hit.Shininess);
		CombinedColor = AmbientColor + DiffuseColor + SpecularColor;

		if (CR_Effects_Shadows) {
			HitInfo LightRayHit;
			for (int i = 0; i < CR_Shapes.size(); ++i) {
				CR_Shapes[i]->CheckIntersection(_Hit.IntPoint + _Hit.Normal * 1e-4f, LightDir, LightRayHit);
			}

			if (LightRayHit.HitStatus && LightRayHit.Distance > 0 && LightRayHit.Distance < glm::length(LightDirFull)) {
				CombinedColor = AmbientColor;
			}
		}

		if (_CurrentDepth < _MaxDepth) {
			HitInfo ReflRayHit;
			return CombinedColor + _Hit.SpecularC * Raytrace(_Hit.ReflRayOrigin, _Hit.ReflRayDir, ReflRayHit, _CurrentDepth += 1, _MaxDepth);
		}
	}
	
	return CombinedColor;
}

void Raytracer::Render()
{
	auto StartTime = std::chrono::high_resolution_clock::now();

	CR_ScreenSurface = SDL_GetWindowSurface(CR_MainWindow);
	CR_ScreenAspectRatio = CR_ScreenSurface->w / CR_ScreenSurface->h;
	CR_BufferSurface = SDL_CreateRGBSurface(0, CR_ScreenSurface->w, CR_ScreenSurface->h, 32, RMask, GMask, BMask, AMask);

	if (CR_BufferSurface != NULL) {
		uint32_t* PixelAddress = (uint32_t*)CR_BufferSurface->pixels;
		if (SDL_MUSTLOCK(CR_BufferSurface)) {
			SDL_LockSurface(CR_BufferSurface);
		}

		int OffsetMod = CR_BufferSurface->pitch * 0.25f;
		float FOV_Angle = glm::tan(glm::radians((float)90 * 0.5f));
		for (int y = 0; y < CR_ScreenSurface->h; ++y) {

			int LineOffset = y * OffsetMod;

			float PixelNormalizedy = (y + 0.5) / CR_ScreenSurface->h;
			float PixelRemappedy = 1 - 2 * PixelNormalizedy;
			float PixelCameray = PixelRemappedy * FOV_Angle;

			for (int x = 0; x < CR_ScreenSurface->w; ++x) {

				/*Uint8* PixelAddress = (Uint8*)ImageSurface->pixels + y * ImageSurface->pitch + x * ImageSurface->format->BytesPerPixel;*/
				/*int LineOffset = y * (ImageSurface->pitch / sizeof(uint32_t));*/				

				float PixelNormalizedx = (x + 0.5) / CR_ScreenSurface->w;
				float PixelRemappedx = (2 * PixelNormalizedx - 1) * CR_ScreenAspectRatio;
				float PixelCamerax = PixelRemappedx * FOV_Angle;

				glm::vec3 PixelCameraSpacePos(PixelCamerax + CR_MainCamera->Position.x, PixelCameray + CR_MainCamera->Position.y, CR_MainCamera->Position.z - 1);
				glm::vec3 RayDirection = glm::normalize(PixelCameraSpacePos - CR_MainCamera->Position);

				Uint32 ColorBitValue = 0;
				HitInfo Hit;
				glm::vec3 CombinedColor = Raytrace(CR_MainCamera->Position, RayDirection, Hit, 1, 1);
				if (CR_Effects_Reflections) {
					glm::vec3 ReflColor = Raytrace(Hit.ReflRayOrigin, Hit.ReflRayDir, Hit, 1, 4);
					CombinedColor = ReflColor;
				}
				ColorBitValue = SDL_MapRGB(CR_BufferSurface->format, glm::clamp(CombinedColor.r * 255, 0.0f, 255.0f), glm::clamp(CombinedColor.g * 255, 0.0f, 255.0f), glm::clamp(CombinedColor.b * 255, 0.0f, 255.0f));
				PixelAddress[LineOffset + x] = ColorBitValue;
			}
		}

		if (SDL_MUSTLOCK(CR_BufferSurface)) {
			SDL_UnlockSurface(CR_BufferSurface);
		}

		SDL_Surface* OptimizedSurface = SDL_ConvertSurface(CR_BufferSurface, CR_ScreenSurface->format, 0);
		SDL_BlitSurface(OptimizedSurface, NULL, CR_ScreenSurface, NULL);
		SDL_FreeSurface(OptimizedSurface);
		OptimizedSurface = NULL;
		SDL_FreeSurface(CR_BufferSurface);
		CR_BufferSurface = NULL;
		SDL_UpdateWindowSurface(CR_MainWindow);
		//std::cout << "Unoptimized Format: " << SDL_GetPixelFormatName(CR_BufferSurface->format->format) << std::endl;
		//std::cout << "Optmized Format: " << SDL_GetPixelFormatName(OptimizedSurface->format->format) << std::endl;
	}
	else {
		std::cout << "WARNING: Unable to render surface! " << std::endl;
		CheckSDLError(__LINE__);
	}

	auto EndTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_seconds = EndTime - StartTime;
	std::cout << "Elapsed time: " << elapsed_seconds.count() << " seconds." << std::endl;
}

void Raytracer::Update()
{
	while (CR_CurrentState == 1) {
		SDL_Event CurrentEvent;
		while (SDL_PollEvent(&CurrentEvent)) {
			if (CurrentEvent.type == SDL_QUIT) {
				CR_CurrentState = INACTIVE;
				break;
			}
			if (CurrentEvent.type == SDL_WINDOWEVENT) {
				switch (CurrentEvent.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					Render();
					break;

				case SDL_WINDOWEVENT_MAXIMIZED:
					Render();
					break;
				default:
					break;
				}
			}
			if (CurrentEvent.type == SDL_KEYDOWN) {
				switch (CurrentEvent.key.keysym.sym) {
				case SDLK_ESCAPE:
					std::cout << "Hotkey Pressed: [ESC]" << std::endl;
					CR_CurrentState = INACTIVE;
					break;
				case SDLK_w:
					CR_MainCamera->Position.z -= 1.0f;			
					std::cout << "Hotkey Pressed: [W]" << std::endl;
					Render();
					break;
				case SDLK_a:
					CR_MainCamera->Position.x -=  1.0f;
					std::cout << "Hotkey Pressed: [A]" << std::endl;
					Render();	
					break;
				case SDLK_d:
					CR_MainCamera->Position.x += 1.0f;
					std::cout << "Hotkey Pressed: [D]" << std::endl;
					Render();
					break;
				case SDLK_s:
					CR_MainCamera->Position.z += 1.0f;
					std::cout << "Hotkey Pressed: [S]" << std::endl;
					Render();
					break;
				case SDLK_1:
					std::cout << "Hotkey Pressed: [1]" << std::endl;
					ToggleShadows();
					std::cout << "Shadows: " << CR_Effects_Shadows << std::endl;
					break;
				case SDLK_2:
					std::cout << "Hotkey Pressed: [2]" << std::endl;
					ToggleReflections();
					std::cout << "Reflections: " << CR_Effects_Reflections << std::endl;
					break;
				default:
					break;
				}
			}
		}

		//glClear(GL_COLOR_BUFFER_BIT);
		//SDL_GL_SwapWindow(CR_MainWindow);
		SDL_UpdateWindowSurface(CR_MainWindow);
	}
}

void Raytracer::ToggleShadows()
{
	CR_Effects_Shadows = !CR_Effects_Shadows;
	Render();
}

void Raytracer::ToggleReflections()
{
	CR_Effects_Reflections = !CR_Effects_Reflections;
	Render();
}

void Raytracer::Deactivate()
{
	for (int i = 0; i < CR_Shapes.size(); ++i) {
		delete CR_Shapes[i];
	}

	SDL_GL_DeleteContext(CR_OGL_Context);
	SDL_FreeSurface(CR_BufferSurface);
	CR_BufferSurface = NULL;
	SDL_FreeSurface(CR_ScreenSurface);
	CR_ScreenSurface = NULL;
	SDL_DestroyWindow(CR_MainWindow);
	CR_MainWindow = NULL;
	SDL_Quit();
}

void Raytracer::PrintProgramLog(GLuint _ProgramID)
{
	if (glIsProgram(_ProgramID)) {
		int InfoLogLength = 0;
		int MaxLength = InfoLogLength;

		glGetProgramiv(_ProgramID, GL_INFO_LOG_LENGTH, &MaxLength);

		char* InfoLog = new char[MaxLength];

		glGetProgramInfoLog(_ProgramID, MaxLength, &InfoLogLength, InfoLog);
		if (InfoLogLength > 0) {
			std::cout << "%s\n" << InfoLog << std::endl;
		}

		delete[] InfoLog;
	}
	else {
		std::cout << "%d is not a valid program ID " << _ProgramID << std::endl;
	}
}

void Raytracer::PrintShaderLog(GLuint _ShaderID)
{
	if (glIsShader(_ShaderID)) {
		int InfoLogLength = 0;
		int MaxLength = InfoLogLength;

		glGetShaderiv(_ShaderID, GL_INFO_LOG_LENGTH, &MaxLength);

		char* InfoLog = new char[MaxLength];

		glGetShaderInfoLog(_ShaderID, MaxLength, &InfoLogLength, InfoLog);
		if (InfoLogLength > 0) {
			std::cout << "%s\n" << InfoLog << std::endl;
		}

		delete[] InfoLog;
	}
	else {
		std::cout << "%d is not a valid shader ID\n" << _ShaderID << std::endl;
	}
}

SDL_Surface* Raytracer::LoadSurface(std::string _ImageFilePath)
{
	SDL_Surface* OptimizedSurface = NULL;

	SDL_Surface* OriginalSurface = SDL_LoadBMP(_ImageFilePath.c_str());
	if (OriginalSurface == NULL) {
		std::cout << "Unable to load image " << std::endl;
		CheckSDLError(__LINE__);
		return NULL;
	}
	OptimizedSurface = SDL_ConvertSurface(OriginalSurface, CR_ScreenSurface->format, 0);
	if (OptimizedSurface == NULL) {
		std::cout << "Unable to optimize loaded surface " << std::endl;
		CheckSDLError(__LINE__);
		return OriginalSurface;
	}
	SDL_FreeSurface(OriginalSurface);

	return OptimizedSurface;
}

void Raytracer::ApplySurfaceToScreen(SDL_Surface* _UpdateSurface, SDL_Surface* _ScreenSurface)
{
	SDL_Rect ScalingRect;
	ScalingRect.x = 0;
	ScalingRect.y = 0;
	ScalingRect.w = CR_WindowWidth;
	ScalingRect.h = CR_WindowHeight;
	SDL_BlitScaled(_UpdateSurface, NULL, _ScreenSurface, &ScalingRect);
}
