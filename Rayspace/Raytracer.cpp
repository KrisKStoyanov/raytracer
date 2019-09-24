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
		std::cout << "WARNING: SDL could not initialize!\n" << std::endl;
		CheckSDLError(__LINE__);
		return 0;
	}

	CR_WindowWidth = _WindowWidth;
	CR_WindowHeight = _WindowHeight;
	CR_ScreenAspectRatio = CR_WindowWidth / CR_WindowHeight;

	CR_MainWindow = SDL_CreateWindow(_WindowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CR_WindowWidth, CR_WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (CR_MainWindow == nullptr) {
		std::cout << "WARNING: Window could not be created!\n" << std::endl;
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
		std::cout << "WARNING: Vsync could not be set!\n" << std::endl;
		CheckSDLError(__LINE__);
	}

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
	std::cout << "." << value << std::endl;
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

	uint32_t RMask, GMask, BMask, AMask;
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

	SDL_Surface* ImageSurface = SDL_CreateRGBSurface(0, CR_WindowWidth, CR_WindowHeight, 32, RMask, GMask, BMask, AMask);
	uint32_t* PixelAddress = (uint32_t*)ImageSurface->pixels;
	int OffsetPrecomputedMod = ImageSurface->pitch * 0.25f;

	bool SurfaceLock = SDL_MUSTLOCK(ImageSurface);
	if (SurfaceLock) {
		SDL_LockSurface(ImageSurface);
	}

	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << CR_WindowWidth << " " << CR_WindowHeight << "\n255\n";
	for (int y = 0; y < CR_WindowHeight; ++y) {
		for (int x = 0; x < CR_WindowWidth; ++x) {

			/*Uint8* PixelAddress = (Uint8*)ImageSurface->pixels + y * ImageSurface->pitch + x * ImageSurface->format->BytesPerPixel;*/
			/*int LineOffset = y * (ImageSurface->pitch / sizeof(uint32_t));*/
			int LineOffset = y * OffsetPrecomputedMod;
			
			float PixelNormalizedx = (x + 0.5) / CR_WindowWidth;
			float PixelNormalizedy = (y + 0.5) / CR_WindowHeight;

			float PixelRemappedx = 2 * PixelNormalizedx - 1;
			float PixelRemappedy = 1 - 2 * PixelNormalizedy;

			PixelRemappedx = (2 * PixelNormalizedx - 1) * CR_ScreenAspectRatio;
			PixelRemappedy = 1 - 2 * PixelNormalizedy;

			float PixelCamerax = PixelRemappedx * glm::tan(glm::radians((float)90 / 2));
			float PixelCameray = PixelRemappedy * glm::tan(glm::radians((float)90 / 2));

			glm::vec3 PcameraSpace(PixelCamerax, PixelCameray, -1);
			glm::vec3 rayDirection = glm::normalize(PcameraSpace - glm::vec3(0, 0, 0));

			float shortestDist = 100000;
			glm::vec3 colorOfShape;
			Uint32 ColorBitValue = 0;
			HitInfo hitInfo;

			for (int i = 0; i < _Shapes.size(); ++i) {
				if (_Shapes[i]->CheckIntersection(rayDirection, glm::vec3(0, 0, 0), hitInfo)) {
					if (hitInfo.ShortestDistance < shortestDist) {
						shortestDist = hitInfo.ShortestDistance;
						colorOfShape = (glm::vec3)hitInfo.Color;
						hitInfo.HitStatus = true;
					}
				}
			}
			if (!hitInfo.HitStatus) {
				ofs << (unsigned char)(182) <<
					(unsigned char)(230) <<
					(unsigned char)(250);
				ColorBitValue = SDL_MapRGB(ImageSurface->format, 182, 230, 250);
			}
			else {
				ofs << (unsigned char)(colorOfShape.r * 255) <<
					(unsigned char)(colorOfShape.g * 255) <<
					(unsigned char)(colorOfShape.b * 255);

				ColorBitValue = SDL_MapRGB(ImageSurface->format, colorOfShape.r * 255, colorOfShape.g * 255, colorOfShape.b * 255);
			}
			PixelAddress[LineOffset + x] = ColorBitValue;
		}
	}
	ofs.close();

	if (ImageSurface != NULL) {
		SDL_Surface* OptimizedSurface = SDL_ConvertSurface(ImageSurface, CR_ScreenSurface->format, 0);

		if (SurfaceLock) {
			SDL_UnlockSurface(ImageSurface);
		}

		CR_ScreenSurface = SDL_GetWindowSurface(CR_MainWindow);
		SDL_BlitSurface(OptimizedSurface, NULL, CR_ScreenSurface, NULL);
		SDL_FreeSurface(ImageSurface);
		SDL_FreeSurface(OptimizedSurface);
		ImageSurface = NULL;
		OptimizedSurface = NULL;
		SDL_UpdateWindowSurface(CR_MainWindow);
	}
	
	return true;
}

void Raytracer::Configure(std::vector<Shape*> _Shapes)
{
	WriteImageToFile(_Shapes);
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
			if (CurrentEvent.type == SDL_KEYDOWN) {
				switch (CurrentEvent.key.keysym.sym) {
				case SDLK_ESCAPE:
					CR_CurrentState = INACTIVE;
					break;
				case SDLK_r:
					glClearColor(1, 0, 0, 1);
					break;
				case SDLK_g:
					glClearColor(0, 1, 0, 1);
					break;
				case SDLK_b:
					glClearColor(0, 0, 1, 1);
					break;
				default:
					break;
				}
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(CR_MainWindow);
		SDL_UpdateWindowSurface(CR_MainWindow);
	}
}

void Raytracer::Deactivate()
{
	UnloadMedia();
	SDL_GL_DeleteContext(CR_OGL_Context);
	SDL_DestroyWindow(CR_MainWindow);
	CR_MainWindow = NULL;
	SDL_FreeSurface(CR_ScreenSurface);
	CR_ScreenSurface = NULL;
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
		std::cout << "%d is not a valid program ID\n" << _ProgramID << std::endl;
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

	//SDL_BlitSurface(CR_MediaSurface, NULL, CR_ScreenSurface, NULL);
	//SDL_UpdateWindowSurface(CR_MainWindow);
}

void Raytracer::UnloadMedia()
{
	SDL_FreeSurface(CR_ScreenSurface);
	CR_ScreenSurface = NULL;
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
