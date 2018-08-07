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

#define WINDOW_ICON "icon.png"
#define WINDOW_TITLE "io-cct"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define UTIL_MAX(a,b)               (((a) > (b)) ? (a) : (b))
#define UTIL_MIN(a,b)               (((a) < (b)) ? (a) : (b))
#define UTIL_CLAMP(lower, x, upper) (UTIL_MIN(upper, UTIL_MAX(x, lower)))

#define CURSOR_ARROW	0
#define CURSOR_SIZE_H	1
#define CURSOR_SIZE_V	2
#define CURSOR_SIZE_ALL 3
#define CURSOR_I_BEAM	4
#define CURSOR_SPECIAL	5 /* The element will handle the cursor */

#define LINE_SPACE 2

class Palette;

class FontHelper;

class Texture;

class Localization;

static const std::string NEW_LINE = "\n";

class SDL_helper
{
public:
	SDL_helper();
	~SDL_helper();

	bool init();
	void close();
	void clear();
	void repaint();

	void util_set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void util_set_color(const SDL_Color * color);

	void util_draw_line(int x1, int y1, int x2, int y2, const SDL_Color * color = nullptr);
	void util_draw_line(const SDL_Point &p1, const SDL_Point &p2, const SDL_Color * color = nullptr);

	void util_draw_rect(const SDL_Rect *rect, const SDL_Color *color = nullptr);
	void util_draw_rect(int x, int y, int w, int h, const SDL_Color * color = nullptr);

	void util_fill_rect(const SDL_Rect * rect, const SDL_Color *color = nullptr);
	void util_fill_rect_shadow(const SDL_Rect *rect, const SDL_Color * color = nullptr, int8_t offset = 3);
	void util_fill_rect(int x, int y, int w, int h, const SDL_Color * color = nullptr);
	void util_fill_rect(const SDL_Rect * rect, const SDL_Color *color, uint8_t alpha);

	bool util_is_in_rect(const SDL_Rect * rect, int x, int y);

	void util_text(const std::string * text, int x, int y, const SDL_Color * color, uint8_t font = FONT_ROBOTO_SMALL);
	void util_text_rot(const std::string * text, int x, int y, const SDL_Color * color, double angle, uint8_t font = FONT_ROBOTO_SMALL);
	SDL_Rect util_text_dim(const std::string * text, uint8_t font = FONT_ROBOTO_SMALL);

	SDL_Point * util_window_size(void);

	void util_cut_string(std::string &s, int max_width, bool front);

	uint8_t util_default_text_height(void) { return m_default_font_height; }
	uint8_t util_wstring_text_height(void) { return m_wstring_font_height; }
	uint8_t util_large_text_height(void) { return m_large_font_height; }

	inline void util_enable_mask(uint16_t & masks, uint16_t mask);
	inline void util_disable_mask(uint16_t & masks, uint16_t mask);

	template<typename ... Args> std::string format(const char * format, Args ... args);

	bool util_check_texture_path(const char * path);

	std::wstring util_utf8_to_wstring(const std::string& str);
	std::string util_wstring_to_utf8(const std::wstring& str);
	
	void format_text(const std::string * s, std::vector<std::unique_ptr<std::string>>& out, SDL_Rect& dim, uint8_t font = FONT_ROBOTO_SMALL);

	std::string loc(const char * id);

	uint32_t vc_to_sdl_key(uint16_t key);
	uint16_t sdl_key_to_vc(uint32_t key);

	SDL_Renderer * renderer()
	{
		return m_sdl_renderer;
	}

	SDL_Window * window()
	{
		return m_sdl_window;
	}

	Palette * palette()
	{
		return m_palette;
	}

	void set_cursor(uint8_t type);

	bool is_ctrl_down(void);
	bool is_shift_down(void);

	void exit_loop(void);
	void set_runflag(bool * flag);
	void handle_events(SDL_Event * event);

	uint8_t util_font_height(uint8_t font);
private:
	TTF_Font * get_font(uint8_t type)
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

	bool m_init_success;

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
};

template<typename ...Args>
inline std::string SDL_helper::format(const char * format, Args ...args)
{
	size_t size = snprintf(nullptr, 0, format, args ...) + 1;
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format, args ...);
	return std::string(buf.get(), buf.get() + size - 1);
}
