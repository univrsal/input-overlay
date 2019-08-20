#ifdef _WIN32
#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>
#include "src/util/sdl_helper.hpp"
#include "src/tool.hpp"

sdl_helper *helper = new sdl_helper();
tool t;

int main(int argc, char **argv)
{
	if (!helper->init()) {
		printf("Initialization failed!\n");
		printf("If loading of resources failed make sure that the following files are located inside the folder '"
			".\\res':\n");
		printf(" unifont.ttf\n icon.png\n");
		printf("<press any key to exit>\n");
		getchar();
		return -1;
	}

	auto texture = "";
	auto config = "";
	if (argc > 2) {
		texture = argv[1];
		config = argv[2];
	}

	t = tool(helper, texture, config);
	t.program_loop();

	helper->close();
	delete helper;
	return 0;
}
