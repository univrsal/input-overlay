#define SDL_MAIN_HANDLED

#include <SDL.h>
#include "src/util/SDL_helper.hpp"

SDL_helper helper;

int main(int argc, char **argv)
{
	SDL_SetMainReady();
	if (!helper.init())
	{
		printf("Initialization failed!\n");
		return -1;
	}
	else
	{
		
	}

	return 0;
}
