#pragma once
#include <string>
#include <array>
#include <mutex>
#include <atomic>
#include <SDL3/SDL.h>

struct input_data;

class sdl_gamepad {
protected:
    std::string m_identifier{};
    SDL_Gamepad *m_controller{};
    int m_player_index{};
    std::atomic<bool> m_valid{true};

    // We save sdl event data into these arrays and copy them in the source tick method
    // Overlay elements should not access these directly
    std::array<bool, SDL_GAMEPAD_BUTTON_COUNT> m_buttons{};
    std::array<float, SDL_GAMEPAD_AXIS_COUNT> m_axis{};
    std::mutex m_mutex{};
    SDL_JoystickID m_jid{};

public:
    explicit sdl_gamepad(SDL_JoystickID jid, std::string const &id)
    {
        m_controller = SDL_OpenGamepad(jid);
        m_jid = jid;
        m_identifier = id;
        m_player_index = SDL_clamp(SDL_GetGamepadPlayerIndex(m_controller), 0, 4);
    }

    operator SDL_Gamepad *() const { return m_controller; }

    SDL_JoystickID index() const { return m_jid; }               // SDL index
    int player_index() const { return m_player_index; } // player 1 through 4

    std::string identifier() const { return m_identifier; }

    void invalidate()
    {
        m_valid = false;
        SDL_CloseGamepad(m_controller);
        m_controller = {};
    }



    bool valid() const { return m_valid; }
    void copy_data(input_data *data, std::string name);

    auto &buttons() { return m_buttons; }
    auto &axis() { return m_axis; }
    auto &mutex() { return m_mutex; }

    ~sdl_gamepad() { invalidate(); }
};
