/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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

#pragma once

#define CCT /* Prevent external headers from including obs headers */

#include <SDL.h>
#include <SDL_ttf.h>

#include <codecvt>
#include <locale>
#include <memory>
#include <vector>

#ifdef LINUX
#include <clocale>
#endif

#include "font_helper.hpp"
#include "util.hpp"

#define CURSOR_ARROW 0
#define CURSOR_SIZE_H 1
#define CURSOR_SIZE_V 2
#define CURSOR_SIZE_ALL 3
#define CURSOR_I_BEAM 4
#define CURSOR_SPECIAL 5 /* The element will handle the cursor */

#define LINE_SPACE 4

class palette;

class font_helper;

class texture;

class localization;

static const std::string new_line = "\n";

class sdl_helper {
public:
    sdl_helper();

    ~sdl_helper();

    bool init();

    void close();

    void clear() const;

    void repaint() const;

    void util_set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const;

    void util_set_color(const SDL_Color *color) const;

    void util_draw_line(int x1, int y1, int x2, int y2, const SDL_Color *color = nullptr) const;

    void util_draw_line(const SDL_Point &p1, const SDL_Point &p2, const SDL_Color *color = nullptr) const;

    void util_draw_rect(const SDL_Rect *rect, const SDL_Color *color = nullptr) const;

    void util_draw_rect(int x, int y, int w, int h, const SDL_Color *color = nullptr) const;

    void util_fill_rect(const SDL_Rect *rect, const SDL_Color *color = nullptr) const;

    void util_fill_rect_shadow(const SDL_Rect *rect, const SDL_Color *color = nullptr, int8_t offset = 3) const;

    void util_fill_rect(int x, int y, int w, int h, const SDL_Color *color = nullptr) const;

    void util_fill_rect(const SDL_Rect *rect, const SDL_Color *color, uint8_t alpha) const;

    static bool util_is_in_rect(const SDL_Rect *rect, int x, int y);

    bool util_mouse_in_rect(const SDL_Rect *rect);

    void util_text(const std::string *text, int x, int y, const SDL_Color *color, uint8_t font = FONT_WSTRING) const;

    void util_text(const std::string *text, int x, int y, const SDL_Color *color, uint8_t font, uint8_t scale) const;

    void util_text_rot(const std::string *text, int x, int y, const SDL_Color *color, double angle,
                       uint8_t font = FONT_WSTRING) const;

    SDL_Rect util_text_dim(const std::string *text, uint8_t font = FONT_WSTRING) const;

    SDL_Point *util_window_size();

    const SDL_Point *util_mouse_pos() const { return &m_mouse_pos; }

    int util_mouse_x() const { return m_mouse_pos.x; }

    int util_mouse_y() const { return m_mouse_pos.y; }

    void util_cut_string(std::string &s, int max_width, bool front) const;

    uint8_t util_large_text_height() const { return m_large_font_height; }

    uint8_t util_default_text_height() const;

    uint8_t util_font_height(uint8_t font) const;

    static void util_open_url(std::string url);

    template<typename... Args> static std::string format(const char *format, Args... args);

    bool util_check_texture_path(const char *path) const;

    bool util_is_windows() const { return m_windows; }

    static std::wstring util_utf8_to_wstring(const std::string &str);

    static std::string util_wstring_to_utf8(const std::wstring &str);

    void format_text(const std::string *s, std::vector<std::unique_ptr<std::string>> &out, SDL_Rect &dim,
                     uint8_t font = FONT_WSTRING) const;

    std::string loc(const char *id) const;

    template<typename... Args> std::string format_loc(const char *unlocalized, Args... args);

    localization *get_localization() const { return m_localization; }

    static uint32_t vc_to_sdl_key(uint16_t key);

    static uint16_t sdl_key_to_vc(uint32_t key);

    SDL_Renderer *renderer() const { return m_sdl_renderer; }

    SDL_Window *window() const { return m_sdl_window; }

    palette *get_palette() const { return m_palette; }

    void init_controllers();

    void set_cursor(uint8_t type) const;

    bool is_ctrl_down() const;

    bool is_shift_down() const;

    void exit_loop() const;

    void set_run_flag(bool *flag);

    void handle_events(SDL_Event *event);

    bool handle_controller_disconnect(int32_t id);

    bool handle_controller_connect(int32_t id);

    void start_frame();

    void end_frame();

    void reset_frame_cap();

    void cap_frame() const;

    float util_get_fps() const;

    static void util_set_flag(uint16_t &flags, const uint16_t mask, const bool state)
    {
        if (state)
            flags |= mask;
        else
            flags &= ~mask;
    }

private:
    TTF_Font *get_font(const uint8_t type) const
    {
        switch (type) {
        case FONT_WSTRING_LARGE:
            return m_large_font;
        default:
        case FONT_WSTRING:
            return m_default_font;
        }
    }

    SDL_Point m_window_size;
    SDL_Point m_mouse_pos;
    SDL_Renderer *m_sdl_renderer;
    SDL_Window *m_sdl_window;

    TTF_Font *m_large_font = nullptr;
    TTF_Font *m_default_font = nullptr;

    uint8_t m_large_font_height = 0;
    uint8_t m_wstring_font_height = 0;

    bool m_focused = true; /* If false, slow down rendering to save resources */
    bool m_init_success = true;
    bool m_windows = true;
    bool *m_runflag = nullptr;

    bool m_ctrl_down = false;
    bool m_shift_down = false;

    palette *m_palette = nullptr;
    font_helper *m_font_helper = nullptr;
    localization *m_localization = nullptr;

    bool m_have_cursors = false;
    SDL_Cursor *m_size_h = nullptr;
    SDL_Cursor *m_size_v = nullptr;
    SDL_Cursor *m_move = nullptr;
    SDL_Cursor *m_i_beam = nullptr;
    SDL_Cursor *m_arrow = nullptr;

    std::vector<SDL_GameController *> m_controllers;

    /* Frame timing */
    timer m_frame_timer;
    timer m_frame_cap_timer;
    float m_fps = 0.f;
    uint32_t m_counted_frames = 0;
};

template<typename... Args> std::string sdl_helper::format(const char *format, Args... args)
{
    const size_t size = snprintf(nullptr, 0, format, args...) + 1ull;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format, args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

template<typename... Args> std::string sdl_helper::format_loc(const char *unlocalized, Args... args)
{
    auto localized = loc(unlocalized);
    const size_t size = snprintf(nullptr, 0, localized.c_str(), args...) + 1ull;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, localized.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}
