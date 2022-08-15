#include "App.h"


int main(int argc, char* argv[])
{
    MySDL::init();
	App app("Test", {1000, 1000}, 10, 20);
	app.start();
	return 0;
}
