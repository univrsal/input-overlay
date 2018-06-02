/**
 * Created by universallp on 23.05.2018.
 * This file is part of input-overlay which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "Palette.hpp"
#include "FontHelper.hpp"
#include "Texture.hpp"

#define WINDOW_TITLE "io-cct"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define UTIL_MAX(a,b)               (((a) > (b)) ? (a) : (b))
#define UTIL_MIN(a,b)               (((a) < (b)) ? (a) : (b))
#define UTIL_CLAMP(lower, x, upper) (UTIL_MIN(upper, UTIL_MAX(x, lower)))

class Palette;

class FontHelper;

class Texture;

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
		void util_set_color(const SDL_Color *color);

		void util_draw_line(int x1, int y1, int x2, int y2, const SDL_Color *color = NULL);
		void util_draw_line(const SDL_Point &p1, const SDL_Point &p2, const SDL_Color *color = NULL);

		void util_draw_rect(const SDL_Rect *rect, const SDL_Color *color = NULL);
		void util_draw_rect(int x, int y, int w, int h, const SDL_Color *color = NULL);

		void util_fill_rect(const SDL_Rect *rect, const SDL_Color *color = NULL);
		void util_fill_rect_shadow(const SDL_Rect *rect, const SDL_Color *color = NULL);
		void util_fill_rect(int x, int y, int w, int h, const SDL_Color *color = NULL);
		void util_fill_rect(const SDL_Rect *rect, const SDL_Color *color, uint8_t alpha);

		bool util_is_in_rect(const SDL_Rect * rect, int x, int y);

		void util_text(std::string * text, int x, int y, const SDL_Color * color);
		void util_text(std::string * text, int x, int y, const SDL_Color * color, double angle);

		SDL_Rect util_text_dim(std::string *text);

		void util_text_utf8(std::string * text, int x, int y, const SDL_Color * color);
		SDL_Rect util_text_utf8_dim(std::string *text);

		SDL_Point * util_window_size(void);

		void util_cut_string(std::string &s, int max_width, bool front);

		void util_enable_mask(uint16_t & masks, uint16_t mask);
		void util_disable_mask(uint16_t & masks, uint16_t mask);

		bool util_check_texture_path(const char * path);

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

		bool is_ctrl_down(void);

		void exit_loop(void);
		void set_runflag(bool * flag);
		void handle_events(SDL_Event * event);
	private:
		SDL_Point m_window_size;
		SDL_Renderer * m_sdl_renderer;
		SDL_Window * m_sdl_window;
		TTF_Font *m_default_font;
		TTF_Font *m_utf8_font;

		bool m_init_success;

		bool * m_runflag = nullptr;

		bool m_ctrl_down = false;
		bool m_shift_down = false;

		Palette *m_palette;
		FontHelper *m_font_helper;
};
