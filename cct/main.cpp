#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <iostream>
#include "src/util/SDL_Helper.hpp"
#include "src/Tool.hpp"

SDL_Helper * helper = new SDL_Helper();;
Tool tool;

int main(int argc, char **argv)
{
	SDL_SetMainReady();

	if (!helper->init())
	{
		printf("Initialization failed!\n");

		printf("If any loading of resources failed make sure that the following files are in the same directory as the exectuable:\n");
		printf(" antique-maru.ttf\n roboto-regular.ttf\n icon.png\n");
		printf("<press any key to exit>\n");
		getchar();
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
