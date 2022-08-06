#include "MySDL.h"

	void MySDL::init() {
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
			throw "SDL_Init video failed";
	}

	SDL_Window* MySDL::createWindow(const char* title, Vector2 dimensions, Uint32 windowFlags) {
		SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dimensions.x, dimensions.y, windowFlags);
		if (!window)
			throw "Failed to create window";
		return window;
	}

	SDL_Renderer* MySDL::createRenderer(SDL_Window* window, int index, Uint32 rendererFlags)
	{
		SDL_Renderer* renderer = SDL_CreateRenderer(window, index, rendererFlags);
		if (!renderer)
			throw "Failed to create renderer";
		return renderer;
	}


Renderer::Renderer(SDL_Renderer* renderer) :
renderer(*renderer)
{}

SDL_Renderer& Renderer::getRenderer() {
	return renderer;
}

Renderer::operator SDL_Renderer*() const {
	return &renderer;
}

void Renderer::setColor(Vector4 color) {
	SDL_SetRenderDrawColor(&renderer, color[0], color[1], color[2], color[3]);
}