#include "Util.hpp"

SDL_bool util_move_element(int* x, int* y, const SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_UP:
        *y = UTIL_MAX(*y - 1, 0);
        return SDL_TRUE;
        break;
    case SDLK_DOWN:
        (*y)++;
        return SDL_TRUE;
        break;
    case SDLK_RIGHT:
        (*x)++;
        return SDL_TRUE;
        break;
    case SDLK_LEFT:
        *x = UTIL_MAX(0, *x - 1);
        return SDL_TRUE;
        break;
    default: ;
    }
    return SDL_FALSE;
}

void util::replace(std::string& str, const char* find, const char* replace)
{
    size_t start = 0;
    const auto len = strlen(find);

    while ((start = str.find(find)) != std::string::npos)
    {
        str.replace(start, len, replace);
    }
}
