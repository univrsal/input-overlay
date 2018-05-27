#define SDL_MAIN_HANDLED

#include <SDL.h>
#include "src/util/SDL_helper.hpp"
#include "src/Tool.hpp"

SDL_helper * helper = new SDL_helper();;
Tool tool;

int main(int argc, char **argv)
{
	SDL_SetMainReady();

	if (!helper->init())
	{
		printf("Initialization failed!\n");
		return -1;
	}
	else
	{
		tool = Tool(helper);
		tool.program_loop();
	}
	helper->close();
	delete helper;
	return 0;
}
