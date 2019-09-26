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

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	// Turn on double buffering with a 24bit Z buffer.
	// May need to change this to 1,16 or 32 based on system
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	CR_OGL_Context = SDL_GL_CreateContext(CR_MainWindow);

	glewExperimental = GL_TRUE;
	GLenum initState = glewInit();

	if (initState != GLEW_OK) {
		std::cout << glewGetErrorString(glewInit()) << std::endl;
		return 0;
	}

	// This makes our buffer swap syncronized with the monitor's vertical refresh - Vsync
	if (SDL_GL_SetSwapInterval(1) < 0) {
		std::cout << "WARNING: Vsync could not be set!" << std::endl;
		CheckSDLError(__LINE__);
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

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(CR_MainWindow);

	PrintOGLVersion();

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
				ofs << (unsigned char)((AmbientC.r * 255) * CR_AmbientLight.r) <<
					(unsigned char)((AmbientC.g * 255) * CR_AmbientLight.g) <<
					(unsigned char)((AmbientC.b * 255) * CR_AmbientLight.b);
			}
		}
	}
	ofs.close();
	
	return true;
}

void Raytracer::Configure(std::vector<Shape*> _Shapes, Camera* _MainCamera, glm::vec3 _AmbientLight, Light* _PointLight)
{
	CR_Shapes = _Shapes;
	CR_MainCamera = _MainCamera;
	CR_AmbientLight = _AmbientLight;
	CR_PointLight = _PointLight;

	Render();
}

void Raytracer::Render()
{
	CR_ScreenSurface = SDL_GetWindowSurface(CR_MainWindow);
	CR_ScreenAspectRatio = CR_ScreenSurface->w / CR_ScreenSurface->h;
	CR_BufferSurface = SDL_CreateRGBSurface(0, CR_ScreenSurface->w, CR_ScreenSurface->h, 32, RMask, GMask, BMask, AMask);

	if (CR_BufferSurface != NULL) {
		uint32_t* PixelAddress = (uint32_t*)CR_BufferSurface->pixels;
		if (SDL_MUSTLOCK(CR_BufferSurface)) {
			SDL_LockSurface(CR_BufferSurface);
		}

		int OffsetPrecomputedMod = CR_BufferSurface->pitch * 0.25f;
		float FOVMod = glm::tan(glm::radians((float)90 / 2));
		for (int y = 0; y < CR_ScreenSurface->h; ++y) {

			int LineOffset = y * OffsetPrecomputedMod;

			float PixelNormalizedy = (y + 0.5) / CR_ScreenSurface->h;
			float PixelRemappedy = 1 - 2 * PixelNormalizedy;
			float PixelCameray = PixelRemappedy * FOVMod;

			for (int x = 0; x < CR_ScreenSurface->w; ++x) {

				/*Uint8* PixelAddress = (Uint8*)ImageSurface->pixels + y * ImageSurface->pitch + x * ImageSurface->format->BytesPerPixel;*/
				/*int LineOffset = y * (ImageSurface->pitch / sizeof(uint32_t));*/				

				float PixelNormalizedx = (x + 0.5) / CR_ScreenSurface->w;
				float PixelRemappedx = (2 * PixelNormalizedx - 1) * CR_ScreenAspectRatio;
				float PixelCamerax = PixelRemappedx * FOVMod;

				glm::vec3 PixelCameraSpacePos(PixelCamerax, PixelCameray, CR_MainCamera->Position.z -1);
				glm::vec3 RayDirection = glm::normalize(PixelCameraSpacePos - CR_MainCamera->Position);

				float DistCheck = 100000;

				glm::vec3 AmbientC;
				glm::vec3 DiffuseC;
				glm::vec3 SpecularC;
				float Shininess = 0;

				glm::vec3 Normal;
				glm::vec3 IntPoint;
				glm::vec3 PrimRayDir;
				glm::vec3 LightReflDir;

				Uint32 ColorBitValue = 0;
				HitInfo Hit;

				for (int i = 0; i < CR_Shapes.size(); ++i) {
					if (CR_Shapes[i]->CheckIntersection(RayDirection, CR_MainCamera->Position, Hit)) {
						if (Hit.Distance < DistCheck) {
							DistCheck = Hit.Distance;

							AmbientC = Hit.AmbientC;
							DiffuseC = Hit.DiffuseC;
							SpecularC = Hit.SpecularC;
							Shininess = Hit.Shininess;

							IntPoint = Hit.IntPoint;
							Normal = Hit.Normal;
							PrimRayDir = Hit.PrimRayDir;

							Hit.HitStatus = true;
						}
					}
				}
				if (!Hit.HitStatus) {
					ColorBitValue = SDL_MapRGB(CR_BufferSurface->format, 182 * CR_AmbientLight.r, 230 * CR_AmbientLight.g, 250 * CR_AmbientLight.b);
				}
				else {
					glm::vec3 LightDir = IntPoint - CR_PointLight->Position;

					LightDir = glm::normalize(LightDir);
					Normal = glm::normalize(Normal);

					PrimRayDir = glm::normalize(PrimRayDir);
					LightReflDir = 2 * (glm::dot(LightDir, Normal)) * Normal - LightDir;
					LightReflDir = glm::normalize(LightReflDir);

					glm::vec3 AmbientColor = AmbientC * CR_AmbientLight;
					glm::vec3 DiffuseColor = DiffuseC * CR_PointLight->ColorIntensity * (glm::max(0.0f, glm::dot(LightDir, Normal)));
					glm::vec3 SpecularColor = SpecularC * CR_PointLight->ColorIntensity * glm::pow((glm::max(0.0f, glm::dot(PrimRayDir, LightReflDir))), Shininess);
					float Red = AmbientColor.r * 255 + DiffuseColor.r * 255 + SpecularColor.r * 255;
					float Green = AmbientColor.g * 255 + DiffuseColor.g * 255 + SpecularColor.g * 255;
					float Blue = AmbientColor.b * 255 + DiffuseColor.b * 255 + SpecularColor.b * 255;
					if (Red > 255) {
						Red = 255;
					}
					if (Green > 255) {
						Green = 255;
					}
					if (Blue > 255) {
						Blue = 255;
					}
					//if (DiffuseColor.r >= 0 && DiffuseColor.g >= 0 && DiffuseColor.b >= 0) {
					//	PhongShadingColor += SpecularColor;
					//}
					ColorBitValue = SDL_MapRGB(CR_BufferSurface->format, Red, Green, Blue);
				}
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
	}
	else {
		std::cout << "WARNING: Unable to render surface! " << std::endl;
		CheckSDLError(__LINE__);
	}
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
					CR_CurrentState = INACTIVE;
					break;
				case SDLK_w:
					CR_MainCamera->Position.z -= 1.0f;
					Render();
					std::cout << "W" << std::endl;
					break;
				case SDLK_a:
					CR_MainCamera->Position.x +=  1.0f;
					Render();
					std::cout << "A" << std::endl;
					break;
				case SDLK_d:
					CR_MainCamera->Position.x -= 1.0f;
					Render();
					std::cout << "D" << std::endl;
					break;
				case SDLK_s:
					CR_MainCamera->Position.z += 1.0f;
					Render();
					std::cout << "S" << std::endl;
					break;
				default:
					break;
				}
			}
		}

		//glClear(GL_COLOR_BUFFER_BIT);
		SDL_UpdateWindowSurface(CR_MainWindow);
		SDL_GL_SwapWindow(CR_MainWindow);

		/*std::cout << "FPS: " << GetFPS() << std::endl;*/
	}
}

void Raytracer::Deactivate()
{
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
