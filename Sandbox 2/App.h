#pragma once
#include "MySDL.h"
#include <iostream>
#include "MyRenderer.h"
#include "Fps.h"
#include "World.h"
class App
{
	int windowFlags = 0;
	int rendererFlags = SDL_RENDERER_ACCELERATED;
	Vector2 windowDimensions;
	MyRenderer renderer;
	Fps fps;
	Fps simulationFps;
	double desiredFramerateDelayMs;
	int frames;
	clock_t lastFrameTime;
	bool running = true;
	int paintRadius = 0;
	World world;
	bool LMBPressed = false;
	Cell* toPlace = &SAND;
	Vector2 mousePos;
public:
	App(const char* windowTitle, Vector2 windowDimensions, int framerate = 120, double scale = 1);

	void handleInput();

	MyRenderer& getRenderer();

	void drawCursor();

	void drawGui();

	void prepareScene();

	void presentScene();

	void renderMap();

	void start();

	void newFramerate(size_t rate);

	~App();
};

