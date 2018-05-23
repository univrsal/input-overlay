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

#include "../util/Palette.hpp"
#include "../util/FontHelper.hpp"

#define WINDOW_TITLE "io-cct"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

class Palette;

class FontHelper;

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

		void util_text(std::string * text, int x, int y, const SDL_Color * color);
		SDL_Rect util_text_dim(std::string *text);

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
	private:
		SDL_Renderer * m_sdl_renderer;
		SDL_Window *m_sdl_window;
		TTF_Font *m_default_font;

		bool m_init_success;
		
		Palette *m_palette;
		FontHelper *m_font_helper;
};
