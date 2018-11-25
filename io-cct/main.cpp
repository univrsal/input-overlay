#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <iostream>
#include "src/util/SDL_Helper.hpp"
#include "src/Tool.hpp"

SDL_Helper * helper = new SDL_Helper();
Tool tool;

int main(int argc, char **argv)
{
    if (!helper->init())
    {
        printf("Initialization failed!\n");
        printf("If loading of resources failed make sure that the following files are located inside the folder '.\res':\n");
        printf(" unifont.ttf\n icon.png\n");
        printf("<press any key to exit>\n");
        getchar();
        return -1;
    }

    auto texture = "";
    auto config = "";
	if (argc > 2)
	{
	    texture = argv[1];
	    config = argv[2];
	}

    tool = Tool(helper, texture, config);
    tool.program_loop();
    

    helper->close();
    delete helper;
    return 0;
}
