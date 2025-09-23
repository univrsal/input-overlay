#include "sdl_gamepad.hpp"
#include <input_data.hpp>

void sdl_gamepad::copy_data(input_data *data, std::string name)
{
    for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; ++i) {
        data->gamepad_buttons[name][i] = SDL_GetGamepadButton(m_controller, (SDL_GamepadButton)i);
    }

    for (int i = 0; i < SDL_GAMEPAD_AXIS_COUNT; ++i) {
        data->gamepad_axis[name][i] =
            SDL_GetGamepadAxis(m_controller, (SDL_GamepadAxis)(i)) / float(INT16_MAX);
    }
}
