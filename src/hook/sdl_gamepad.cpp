#include "sdl_gamepad.hpp"
#include <input_data.hpp>

void sdl_gamepad::copy_data(input_data *data)
{
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_TOUCHPAD; ++i) {
        data->gamepad_buttons[m_index][i] = SDL_GameControllerGetButton(m_controller, (SDL_GameControllerButton)i) ==
                                            SDL_PRESSED;
    }

    for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; ++i) {
        data->gamepad_axis[m_index][i] =
            SDL_GameControllerGetAxis(m_controller, (SDL_GameControllerAxis)(i)) / float(INT16_MAX);
    }
}
