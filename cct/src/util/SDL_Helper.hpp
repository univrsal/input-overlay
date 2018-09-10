/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <codecvt>
#include <locale>
#include <iostream>
#include <vector>
#include <memory>
#ifdef LINUX
#include <clocale>
#endif

#include "Palette.hpp"
#include "FontHelper.hpp"
#include "Texture.hpp"
#include "Util.hpp"

#define CURSOR_ARROW    0
#define CURSOR_SIZE_H   1
#define CURSOR_SIZE_V   2
#define CURSOR_SIZE_ALL 3
#define CURSOR_I_BEAM   4
#define CURSOR_SPECIAL  5 /* The element will handle the cursor */

#define LINE_SPACE      4

class Palette;

class FontHelper;

class Texture;

class Localization;

static const std::string NEW_LINE = "\n";

class SDL_Helper
{
public:
    SDL_Helper();
    ~SDL_Helper();

    bool init();
    void close();
    void clear() const;
    void repaint() const;

    void util_set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const;
    void util_set_color(const SDL_Color * color) const;

    void util_draw_line(int x1, int y1, int x2, int y2, const SDL_Color * color = nullptr) const;
    void util_draw_line(const SDL_Point &p1, const SDL_Point &p2, const SDL_Color * color = nullptr) const;

    void util_draw_rect(const SDL_Rect *rect, const SDL_Color *color = nullptr) const;
    void util_draw_rect(int x, int y, int w, int h, const SDL_Color * color = nullptr) const;

    void util_fill_rect(const SDL_Rect * rect, const SDL_Color *color = nullptr) const;
    void util_fill_rect_shadow(const SDL_Rect *rect, const SDL_Color * color = nullptr, int8_t offset = 3) const;
    void util_fill_rect(int x, int y, int w, int h, const SDL_Color * color = nullptr) const;
    void util_fill_rect(const SDL_Rect * rect, const SDL_Color *color, uint8_t alpha) const;

    bool util_is_in_rect(const SDL_Rect * rect, int x, int y) const;

    void util_text(const std::string * text, int x, int y, const SDL_Color * color, uint8_t font = FONT_ROBOTO_SMALL) const;
    void util_text_rot(const std::string * text, int x, int y, const SDL_Color * color, double angle, uint8_t font = FONT_ROBOTO_SMALL) const;
    SDL_Rect util_text_dim(const std::string * text, uint8_t font = FONT_ROBOTO_SMALL) const;

    SDL_Point * util_window_size(void);

    void util_cut_string(std::string &s, int max_width, bool front) const;

    uint8_t util_default_text_height(void) const { return m_default_font_height; }
    uint8_t util_wstring_text_height(void) const { return m_wstring_font_height; }
    uint8_t util_large_text_height(void) const { return m_large_font_height; }

    template<typename ... Args> std::string format(const char * format, Args ... args);

    bool util_check_texture_path(const char * path) const;

    std::wstring util_utf8_to_wstring(const std::string& str) const;
    std::string util_wstring_to_utf8(const std::wstring& str) const;
    
    void format_text(const std::string * s, std::vector<std::unique_ptr<std::string>>& out, SDL_Rect& dim, uint8_t font = FONT_ROBOTO_SMALL) const;

    std::string loc(const char * id) const;
    template<typename ... Args> std::string format_loc(const char * unlocalized, Args ... args);

    Localization * localization(void) const { return m_localization; }

    uint32_t vc_to_sdl_key(uint16_t key) const;
    uint16_t sdl_key_to_vc(uint32_t key) const;

    SDL_Renderer * renderer() const
    {
        return m_sdl_renderer;
    }

    SDL_Window * window() const
    {
        return m_sdl_window;
    }

    Palette * palette() const
    {
        return m_palette;
    }

    void init_controllers(void);
    void set_cursor(uint8_t type) const;

    bool is_ctrl_down(void) const;
    bool is_shift_down(void) const;

    void exit_loop(void) const;
    void set_runflag(bool * flag);
    void handle_events(SDL_Event * event);

    uint8_t util_font_height(uint8_t font) const;

    bool handle_controller_disconnect(uint8_t id);
    bool handle_controller_connect(uint8_t id);

    void start_frame(void);
    void end_frame(void);
    void cap_frame(void) const;
    float util_get_fps(void) const;
private:
    TTF_Font * get_font(uint8_t type) const
    {
        switch (type)
        {
            default:
            case FONT_ROBOTO_SMALL:
                return m_default_font;
            case FONT_ROBOTO_LARGE:
                return m_large_font;
            case FONT_WSTRING:
                return m_wstring_font;
        }
    }


    SDL_Point m_window_size;
    SDL_Renderer * m_sdl_renderer = nullptr;
    SDL_Window * m_sdl_window = nullptr;

    TTF_Font * m_default_font = nullptr;
    TTF_Font * m_large_font = nullptr;
    TTF_Font * m_wstring_font = nullptr;

    uint8_t m_default_font_height;
    uint8_t m_large_font_height;
    uint8_t m_wstring_font_height;

    bool m_init_success = true;

    bool * m_runflag = nullptr;

    bool m_ctrl_down = false;
    bool m_shift_down = false;

    Palette * m_palette = nullptr;
    FontHelper * m_font_helper = nullptr;
    Localization * m_localization = nullptr;

    bool m_have_cursors = false;
    SDL_Cursor * m_size_h = nullptr;
    SDL_Cursor * m_size_v = nullptr;
    SDL_Cursor * m_move = nullptr;
    SDL_Cursor * m_i_beam = nullptr;
    SDL_Cursor * m_arrow = nullptr;

    std::vector<SDL_GameController*> m_controllers;

    /* Frame timing */
    Timer m_frame_timer;
    Timer m_frame_cap_timer;
    float m_fps = 0.f;
    uint32_t m_counted_frames = 0;
};

template<typename ...Args>
inline std::string SDL_Helper::format(const char * format, Args ... args)
{
    size_t size = snprintf(nullptr, 0, format, args ...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format, args ...);
    return std::string(buf.get(), buf.get() + size - 1);
}

template<typename ...Args>
inline std::string SDL_Helper::format_loc(const char * unlocalized, Args ... args)
{
    std::string localized = loc(unlocalized);
    size_t size = snprintf(nullptr, 0, localized.c_str(), args ...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, localized.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1);
}
