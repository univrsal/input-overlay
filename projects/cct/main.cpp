/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#ifdef _WIN32
#define SDL_MAIN_HANDLED
#endif

#include "src/tool.hpp"
#include "src/util/sdl_helper.hpp"
#include <SDL.h>

sdl_helper helper;
tool t;

int main(int argc, char **argv)
{
    if (!helper.init()) {
        printf("Initialization failed!\n");
        printf("If loading of resources failed make sure that the following"
               "files are located inside the folder '.\\res':\n");
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

    t = tool(&helper, texture, config);
    t.program_loop();
    return 0;
}
