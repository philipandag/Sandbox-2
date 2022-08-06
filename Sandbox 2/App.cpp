#include "App.h"

App::App(const char* windowTitle, Vector2 windowDimensions, int framerate, double scale):
	renderer(windowTitle, windowDimensions, windowFlags, rendererFlags),
	fps(framerate),
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
		int mousex, mousey;
		SDL_GetMouseState(&mousex, &mousey);
		int posx = mousex / world.scale;
		int posy = mousey / world.scale;
		for (size_t x = posx - paintRadius; x <= posx + paintRadius; x++)
			for (size_t y = posy - paintRadius; y <= posy + paintRadius; y++) {
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

void App::presentScene()
{
	//renderer.renderTexture(world.texture);
	//SDL_Rect r;
	//r.w = world.dimensions.x;
	//r.h = world.dimensions.y;
	//SDL_RenderCopy(renderer.getRenderer(), world.texture, NULL, NULL);
	char text[128];
	sprintf_s(text, "Cells count: %d\nChunks count: %d\nFPS: %d/%d\nBrush size: %d\nMaterial: %s",
		world.cellsCount, world.chunks.size(), fps.getFPS(), fps.getDesiredFramerate(), paintRadius, toPlace->name);
	renderer.write(text);

	SDL_RenderPresent(renderer.getRenderer());
	renderer.setColor({ 0,0,0,0 });
	renderer.fill();
	fps++;
}

void App::start()
{
	presentScene();
	while (running)
	{
		handleInput();
		prepareScene();
		fps.delay();
		presentScene();
	}
}

void App::newFramerate(size_t rate)
{
	fps.newFramerate(rate);
}
