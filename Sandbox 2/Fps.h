#pragma once
#include <time.h>
#include "MySDL.h"
#include <iostream>
using std::ostream;

class Fps
{
	clock_t frameStart;
	clock_t counterStart;
	clock_t frameDelta;
	clock_t counterDelta;
	unsigned int frames;
	unsigned int updateDelayMs;
	unsigned int desiredFramerate;
	unsigned int desiredDelay;

	int currentFPS;
public:
	Fps(int desiredFramerate = 60, int updateDelayMs = 1000):
		frameStart(clock()),
		counterStart(clock()),
		frameDelta(0),
		counterDelta(0),
		updateDelayMs(updateDelayMs),
		desiredFramerate(desiredFramerate)
	{
		desiredDelay = 1000.0 / desiredFramerate;
	}

	void operator++(int)
	{
		frames++;
		update();
	}

	void update()
	{
		if (clock() - counterStart >= updateDelayMs)
		{
			currentFPS = (float)frames / updateDelayMs * 1000.0;
			frames = 0;
			counterStart += updateDelayMs;
		}
	}

	void delay()
	{
		if ((clock() - frameStart) < desiredDelay)
		{
			SDL_Delay(desiredDelay - (clock() - frameStart));
		}
		frameStart = clock();
	}

	void newFramerate(size_t rate)
	{
		desiredFramerate = rate;
		desiredDelay = 1000.0 / rate;
	}

	size_t getDesiredFramerate() {
		return desiredFramerate;
	}

	int getFPS() {
		return currentFPS;
	}
};



