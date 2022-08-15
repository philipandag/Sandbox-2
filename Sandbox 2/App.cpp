#include "App.h"

App::App(const char* windowTitle, Vector2 windowDimensions, int framerate, double scale):
	renderer(windowTitle, windowDimensions, windowFlags, rendererFlags),
	fps(75),
	simulationFps(framerate),
	windowDimensions(windowDimensions),
	world(windowDimensions, scale, renderer)
{
	int windowFlags = 0;
	int rendererFlags = SDL_RENDERER_ACCELERATED;
	Randomiser::initRandomiser();
	SDL_SetWindowSize(renderer.getWindow(), world.chunkDimensions.x * chunkDensity * scale, world.chunkDimensions.y * chunkDensity * scale);
}

void App::handleInput()
{
	SDL_GetMouseState(&mousePos.x, &mousePos.y);
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			running = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			LMBPressed = true;
			break;
		case SDL_MOUSEBUTTONUP:
			LMBPressed = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_0:
				toPlace = &EMPTY;
				break;
			case SDLK_1:
				toPlace = &SAND;
				break;
			case SDLK_2:
				toPlace = &WATER;
				break;
			case SDLK_3:
				toPlace = &ROCK;
				break;
			case SDLK_4:
				toPlace = &WALL;
				break;
			case SDLK_5:
				toPlace = &WEIRDO;
				break;
			case SDLK_UP:
				newFramerate(fps.getDesiredFramerate() + 1);
				break;
			case SDLK_DOWN:
				newFramerate(fps.getDesiredFramerate() - 1);
				break;
			case SDLK_LEFT:
				paintRadius = paintRadius == 0 ? 0 : paintRadius-1;
				break;
			case SDLK_RIGHT:
				paintRadius++;
				break;
			case SDLK_p:
				if (fps.getDesiredFramerate() == 1)
					newFramerate(10);
				else
					newFramerate(1);
				break;
			}
			break;
		case SDL_MOUSEWHEEL:
			world.scale += event.wheel.y;
		default:
			break;
		}
	}

	if (LMBPressed)
	{
		int posx = mousePos.x / world.scale;
		int posy = mousePos.y / world.scale;
		for (int x = posx - paintRadius; x <= posx + paintRadius; x++)
			for (int y = posy - paintRadius; y <= posy + paintRadius; y++) {
				if (world.canGoHere(x, y, toPlace))
					world.setCell(x, y, toPlace);
			}
		//LMBPressed = false;
	}
}

MyRenderer& App::getRenderer() {
	return renderer;
}

App::~App()
{
	SDL_Quit();
}

void App::prepareScene()
{
	world.update();
}

void App::drawCursor()
{
	SDL_SetRenderDrawColor(renderer.getRenderer(), 255, 255, 255, 255);
	SDL_Rect r;
	r.x = (mousePos.x / static_cast<int>(world.scale)) * world.scale - (paintRadius)*world.scale;
	r.y = (mousePos.y / static_cast<int>(world.scale)) * world.scale - (paintRadius)*world.scale;
	r.w = (paintRadius * 2 + 1) * world.scale;
	r.h = (paintRadius * 2 + 1) * world.scale;
	SDL_RenderDrawRect(renderer.getRenderer(), &r);
}

void App::drawGui()
{
	char text[128];
	sprintf_s(text, "Cells count: %d\nChunks count: %d\nFPS: %d/%d\nBrush size: %d\nMaterial: %s",
		world.cellsCount, world.chunks.size(), fps.getFPS(), fps.getDesiredFramerate(), paintRadius, toPlace->name);
	renderer.write(text);

	drawCursor();
}

void App::presentScene()
{
	renderer.setColor({ 0,0,0,0 });
	renderer.fill();
	
	

	//SDL_RenderCopy(renderer.getRenderer(), world.texture, NULL, NULL);
	drawGui();
	SDL_RenderPresent(renderer.getRenderer());
}

void App::start()
{
	presentScene();
	while (running)
	{
		handleInput();
		if (simulationFps.delayPassed())
		{
			prepareScene();
			simulationFps++;
		}
		fps.delay();
		presentScene();
		fps++;
	}
}

void App::newFramerate(size_t rate)
{
	fps.newFramerate(rate);
}
