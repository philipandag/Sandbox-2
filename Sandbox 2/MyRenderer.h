#pragma once
#include "MySDL.h"
#include "Vector2.h"
#include "Vector4.h"
#include <vector>
#include "Writer.h"
using std::vector;

class MyRenderer
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	Writer* writer;
public:
	MyRenderer(const char* windowTitle, Vector2 windowDimensions, int windowFlags, int rendererFlags) :
		window(MySDL::createWindow(windowTitle, windowDimensions, windowFlags)),
		renderer(MySDL::createRenderer(window, -1, rendererFlags)),
		writer(new Writer(renderer))
	{
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	}

	void clear() {
		SDL_RenderClear(renderer);
	}

	void setColor(Vector4 color = { 0, 0, 0, 255 })
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	}

	void fill()
	{
		SDL_RenderFillRect(renderer, NULL);
	}

	void renderTexture(SDL_Texture* texture)
	{
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	void drawRect(SDL_Rect& rect, Vector4 color = { 0, 0, 0, 255 })
	{
		setColor(color);
		SDL_RenderFillRect(renderer, &rect);
	}

	SDL_Renderer* getRenderer() {
		return renderer;
	}

	SDL_Window* getWindow() {
		return window;
	}

	void write(const char* text, int x = 0, int y = 0) {
		writer->write(text, x, y);
	}

};