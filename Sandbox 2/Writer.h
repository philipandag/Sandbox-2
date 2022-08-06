#pragma once
#include <fstream>
#include <SDL.h>
#include "Vector2.h"
using namespace std;
class Writer
{
	SDL_Texture* charactersTexture;
	SDL_Texture* textTexture;
	SDL_Renderer* renderer;
	const Vector2 CHARACTER_DIMENSIONS = { 8,8 };
	Vector2 textTextureDimensionsInCharacters;
	const int MAX_TEXT_LEN = 128;
	const int MAX_TEXT_LINES = 32;
public:
	Writer(SDL_Renderer* r, const char* path = "bmp/cs8x8_transparent.bmp"):
		charactersTexture(nullptr),
		textTexture(nullptr),
		renderer(r)
	{
		textTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, CHARACTER_DIMENSIONS.x * MAX_TEXT_LEN, CHARACTER_DIMENSIONS.y * MAX_TEXT_LINES);
		SDL_Surface* charactersSurface = SDL_LoadBMP_RW(SDL_RWFromFile(path, "rb"), 1);

		if (!charactersSurface)
		{
			printf("Writer - Unable to load %s", path);
			return;
		}
		charactersTexture = SDL_CreateTextureFromSurface(renderer, charactersSurface);
		SDL_QueryTexture(charactersTexture, NULL, NULL, &textTextureDimensionsInCharacters.x, &textTextureDimensionsInCharacters.y);
		textTextureDimensionsInCharacters.x /= CHARACTER_DIMENSIONS.x;
		textTextureDimensionsInCharacters.y /= CHARACTER_DIMENSIONS.y;
		SDL_FreeSurface(charactersSurface);
	}	

	void write(const char* text, int x, int y)
	{
		int startx = x;
		int starty = y;
		char character;
		SDL_Rect rectBMP, rectScreen;
		rectBMP.w = CHARACTER_DIMENSIONS.x;
		rectBMP.h = CHARACTER_DIMENSIONS.y;
		rectScreen.w = CHARACTER_DIMENSIONS.x;
		rectScreen.h = CHARACTER_DIMENSIONS.y;

		while(*text)
		{
			character = *text;
			if (character == '\n')
			{
				y += CHARACTER_DIMENSIONS.y;
				x = startx;
				text++;
				continue;
			}

			rectBMP.x = (character % textTextureDimensionsInCharacters.x) * CHARACTER_DIMENSIONS.x;
			rectBMP.y = (character / textTextureDimensionsInCharacters.y) * CHARACTER_DIMENSIONS.y;
			rectScreen.x = x;
			rectScreen.y = y;
			SDL_RenderCopy(renderer, charactersTexture, &rectBMP, &rectScreen);
			x += CHARACTER_DIMENSIONS.x;
			text++;
		}
	}
};

