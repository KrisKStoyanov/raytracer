#include <iostream>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

const int WIDTH = 800;
const int HEIGHT = 600;

int main(int argc, char* argv[]) {

	float ScreenAspectRatio = WIDTH / HEIGHT;

	for (int x = 0; x < WIDTH; ++x) {
		for (int y = 0; y < HEIGHT; ++y) {

			float PixelNormalizedx = (x + 0.5) / WIDTH;
			float PixelNormalizedy = (y + 0.5) / WIDTH;

			float PixelRemappedx = 2 * PixelNormalizedx - 1;
			float PixelRemappedy = 1 - 2 * PixelNormalizedy;

			PixelRemappedx = (2 * PixelNormalizedx - 1) * ScreenAspectRatio;
			PixelRemappedy = 1 - 2 * PixelNormalizedy;

			float PixelCamerax = PixelRemappedx * glm::tan(glm::radians(90) / 2);
		}
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("Cyberspace", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (window == nullptr) {
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		SDL_Log("Could not create a renderer: %s", SDL_GetError());
		return -1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	GLenum initState = glewInit();

	if (initState != GLEW_OK) {
		std::cout << glewGetErrorString(glewInit()) << std::endl;
	}

	while (true)
	{
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_QUIT:
				exit(0);
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					exit(0);
					break;
				}
				break;
			}
		}

		//Uint8 redVal = rand() % 255;
		//Uint8 greenVal = rand() % 255;
		//Uint8 blueVal = rand() % 255;

		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
		SDL_RenderClear(renderer);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}