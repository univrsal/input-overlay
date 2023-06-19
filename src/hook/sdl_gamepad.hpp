#pragma once
#include <string>
#include <array>
#include <mutex>
#include <atomic>
#include "SDL.h"

struct input_data;

class sdl_gamepad {
protected:
    std::string m_identifier{};
    SDL_GameController *m_controller{};
    int m_index{}, m_player_index{};
    std::atomic<bool> m_valid{true};

    // We save sdl event data into these arrays and copy them in the source tick method
    // Overlay elements should not access these directly
    std::array<bool, SDL_CONTROLLER_BUTTON_MAX> m_buttons{};
    std::array<float, SDL_CONTROLLER_AXIS_MAX> m_axis{};
    std::mutex m_mutex{};

public:
    explicit sdl_gamepad(int index, std::string const &id)
    {
        m_controller = SDL_GameControllerOpen(index);
        m_identifier = id;
        m_player_index = SDL_clamp(SDL_GameControllerGetPlayerIndex(m_controller), 0, 4);
    }

    operator SDL_GameController *() const { return m_controller; }

    int index() const { return m_index; }               // SDL index
    int player_index() const { return m_player_index; } // player 1 through 4

    std::string identifier() const { return m_identifier; }

    void invalidate()
    {
        m_valid = false;
        SDL_GameControllerClose(m_controller);
        m_controller = {};
    }

    bool valid() const { return m_valid; }
    void copy_data(input_data *data, int index);

    auto &buttons() { return m_buttons; }
    auto &axis() { return m_axis; }
    auto &mutex() { return m_mutex; }

    ~sdl_gamepad() { invalidate(); }
};
