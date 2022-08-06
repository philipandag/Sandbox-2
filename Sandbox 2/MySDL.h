#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <SDL.h>

namespace MySDL{
	
	void init();

	SDL_Window* createWindow(const char* title, Vector2 dimensions, Uint32 windowFlags);

	SDL_Renderer* createRenderer(SDL_Window* window, int index, Uint32 rendererFlags);

};

class Renderer
{
	SDL_Renderer& renderer;
public:
	Renderer(SDL_Renderer* renderer);

	SDL_Renderer& getRenderer();

	operator SDL_Renderer* () const;

	void setColor(Vector4 color);
};

