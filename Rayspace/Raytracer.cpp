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

	CR_MainWindow = SDL_CreateWindow(_WindowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CR_WindowWidth, CR_WindowHeight, SDL_WINDOW_OPENGL);
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

bool Raytracer::WriteImageToFile(std::vector<Shape*> _Shape) {
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
			glm::vec3 rayDirection = glm::normalize(PcameraSpace - glm::vec3(0, 0, 0));

			HitInfo hitInfo;

			Shape* closestShape = nullptr;
			float shortestDist = 100000;
			glm::vec3 colorOfShape;
			for (int i = 0; i < _Shape.size(); ++i) {
				if (_Shape[i]->CheckIntersection(rayDirection, glm::vec3(0, 0, 0), hitInfo)) {
					if (hitInfo.distT < shortestDist) {
						shortestDist = hitInfo.distT;
						colorOfShape = hitInfo.color;
						closestShape = _Shape[i];
					}
				}
			}

			if (closestShape == nullptr) {
				ofs << (unsigned char)(182) <<
					(unsigned char)(230) <<
					(unsigned char)(250);
			}
			else {
				ofs << (unsigned char)(colorOfShape.r * 255) <<
					(unsigned char)(colorOfShape.g * 255) <<
					(unsigned char)(colorOfShape.b * 255);
			}

		}
	}
	ofs.close();
	return true;
}

void Raytracer::Configure()
{

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

		//for (int i = 0; i < CR_Entities.size(); ++i) {
		//	CR_Entities[i]->Render();
		//}

		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(CR_MainWindow);
	}
}

void Raytracer::Deactivate()
{
	UnloadMedia();
	SDL_GL_DeleteContext(CR_OGL_Context);
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

bool Raytracer::LoadMedia(std::string _ImageFilePath)
{
	CR_MediaSurface = SDL_LoadBMP(_ImageFilePath.c_str());
	if (CR_MediaSurface == NULL) {
		std::cout << "Unable to load image " << std::endl;
		CheckSDLError(__LINE__);
		return false;
	}
	SDL_BlitSurface(CR_MediaSurface, NULL, CR_ScreenSurface, NULL);
	SDL_UpdateWindowSurface(CR_MainWindow);
	return true;
}

void Raytracer::UnloadMedia()
{
	SDL_FreeSurface(CR_MediaSurface);
	CR_MediaSurface = NULL;
	SDL_FreeSurface(CR_ScreenSurface);
	CR_ScreenSurface = NULL;
}
