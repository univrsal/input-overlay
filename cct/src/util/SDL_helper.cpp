/**
 * Created by universallp on 23.05.2018.
 * This file is part of input-overlay which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "SDL_helper.hpp"

SDL_helper::SDL_helper()
{
	m_sdl_renderer = NULL;
	m_sdl_window = NULL;
	m_init_success = true;
	m_default_font = NULL;
	m_font_helper = NULL;
	m_palette = NULL;
}

SDL_helper::~SDL_helper()
{
	close();
	delete m_palette;
	delete m_font_helper;

	m_font_helper = NULL;
	m_sdl_renderer = NULL;
	m_sdl_window = NULL;
	m_default_font = NULL;
	m_palette = NULL;
}

bool SDL_helper::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Initialization of SDL failed! Error: %s\n", SDL_GetError());
		m_init_success = false;
	}

	m_sdl_window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (m_sdl_window == NULL) {
		printf("Creating SDL Window failed! Error: %s\n", SDL_GetError());
		m_init_success = false;
	}
	else
	{
		SDL_SetWindowMinimumSize(m_sdl_window, WINDOW_WIDTH, WINDOW_HEIGHT);
		m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	}

	if (m_sdl_renderer == NULL) {
		printf("Creating SDL Renderer failed! Error: %s\n", SDL_GetError());
		m_init_success = false;
	}
	else
	{
		SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
		SDL_GetWindowSize(m_sdl_window, &m_window_size.x, &m_window_size.y);
	}

	if (TTF_Init() == -1) {
		printf("Initializing SDL_ttf failed! Error: %s\n", TTF_GetError());
		m_init_success = false;
	}
	else {

		m_default_font = TTF_OpenFont("./roboto-regular.ttf", FONT_DEFAULT);
		m_utf8_font = TTF_OpenFont("./antique-maru.ttf", FONT_DEFAULT);

		if (!m_default_font || !m_utf8_font)
		{
			printf("Couldn't load fonts!\n");
			m_init_success = false;
		}
		else
		{
			m_font_helper = new FontHelper(this);
		}
	}

	m_palette = new Palette();
	return m_init_success;
}

void SDL_helper::close()
{
	SDL_DestroyRenderer(m_sdl_renderer);
	SDL_DestroyWindow(m_sdl_window);


	if (m_default_font)
		TTF_CloseFont(m_default_font);
	if (m_utf8_font)
		TTF_CloseFont(m_utf8_font);

	m_default_font = nullptr;
	m_utf8_font = nullptr;

	TTF_Quit();
	SDL_Quit();
}

void SDL_helper::clear()
{
	util_set_color(m_palette->dark_gray());
	SDL_RenderClear(m_sdl_renderer);
}

void SDL_helper::repaint()
{
	SDL_RenderPresent(m_sdl_renderer);
}

void SDL_helper::util_set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
}

void SDL_helper::util_set_color(const SDL_Color * color)
{
	if (color == NULL)
		util_set_color(m_palette->white()->r, m_palette->white()->g, m_palette->white()->b, m_palette->white()->a);
	else
		util_set_color(color->r, color->g, color->b, color->a);
}

void SDL_helper::util_draw_line(int x1, int y1, int x2, int y2, const SDL_Color * color)
{
	util_set_color(color);
	SDL_RenderDrawLine(m_sdl_renderer, x1, y1, x2, y2);
}

void SDL_helper::util_draw_line(const SDL_Point & p1, const SDL_Point & p2, const SDL_Color * color)
{
	util_set_color(color);
	SDL_RenderDrawLine(m_sdl_renderer, p1.x, p1.y, p2.x, p2.y);
}

void SDL_helper::util_draw_rect(const SDL_Rect * rect, const SDL_Color * color)
{
	util_set_color(color);
	SDL_RenderDrawRect(m_sdl_renderer, rect);
}

void SDL_helper::util_draw_rect(int x, int y, int w, int h, const SDL_Color * color)
{
	SDL_Rect temp_rect = { x, y, w, h };
	util_set_color(color);
	SDL_RenderDrawRect(m_sdl_renderer, &temp_rect);
}

void SDL_helper::util_fill_rect(const SDL_Rect * rect, const SDL_Color * color)
{
	util_set_color(color);
	SDL_RenderFillRect(m_sdl_renderer, rect);
}

void SDL_helper::util_fill_rect_shadow(const SDL_Rect * rect, const SDL_Color * color)
{
	SDL_Rect temp_rect = { rect->x + 3, rect->y + 3, rect->w, rect->h };
	util_fill_rect(&temp_rect, m_palette->black());
	util_fill_rect(rect, color);
}

void SDL_helper::util_fill_rect(int x, int y, int w, int h, const SDL_Color * color)
{
	util_set_color(color);
	SDL_Rect temp_rect = SDL_Rect{ x, y, w, h };
	SDL_RenderFillRect(m_sdl_renderer, &temp_rect);
}

void SDL_helper::util_fill_rect(const SDL_Rect * rect, const SDL_Color * color, uint8_t alpha)
{
	SDL_Color temp;
	temp.a = alpha;
	temp.r = color->r;
	temp.g = color->g;
	temp.b = color->b;
	util_fill_rect(rect, &temp);
}

bool SDL_helper::util_is_in_rect(const SDL_Rect * rect, int x, int y)
{
    return x >= rect->x && x <= (rect->x + rect->w) && y >= rect->y && y <= (rect->y + rect->h);
}

void SDL_helper::util_text(std::string * text, int x, int y, const SDL_Color * color)
{
	if (color == NULL)
		m_font_helper->draw(text, x, y, m_default_font, m_palette->white());
	else
		m_font_helper->draw(text, x, y, m_default_font, color);
}

void SDL_helper::util_text(std::string * text, int x, int y, const SDL_Color * color, double angle)
{
	if (color == NULL)
		m_font_helper->draw_rot(text, x, y, m_default_font, m_palette->white(), angle);
	else
		m_font_helper->draw_rot(text, x, y, m_default_font, color, angle);
}

SDL_Rect SDL_helper::util_text_dim(std::string * text)
{
	return m_font_helper->get_text_dimension(m_default_font, text);
}

void SDL_helper::util_text_utf8(std::string * text, int x, int y, const SDL_Color * color)
{
	if (color == NULL)
		m_font_helper->draw(text, x, y, m_utf8_font, m_palette->white());
	else
		m_font_helper->draw(text, x, y, m_utf8_font, color);
}

SDL_Rect SDL_helper::util_text_utf8_dim(std::string * text)
{
	return m_font_helper->get_text_dimension(m_utf8_font, text);
}

SDL_Point * SDL_helper::util_window_size(void)
{
	return &m_window_size;
}

void SDL_helper::util_cut_string(std::string & s, int max_width, bool front)
{
	int w = util_text_dim(&s).w;
	bool was_cut = false;
	while (w > max_width)
	{
		if (front)
			s = s.substr(0, s.size() - 1);
		else
			s = s.substr(1, s.size());
		w = util_text_dim(&s).w;
		was_cut = true;
	}

	if (was_cut)
	{
		if (front)
			s.append("...");
		else
			s = "..." + s;
	}
}

void SDL_helper::util_enable_mask(uint16_t & masks, uint16_t mask)
{
	masks |= mask;
}

void SDL_helper::util_disable_mask(uint16_t & masks, uint16_t mask)
{
	masks &= ~mask;
}

bool SDL_helper::util_check_texture_path(const char * path)
{
	Texture temp;
	bool flag = temp.load(path, m_sdl_renderer);
	temp.free();
	return flag;
}

bool SDL_helper::is_ctrl_down(void)
{
	return m_ctrl_down;
}

void SDL_helper::exit_loop(void)
{
	*m_runflag = false;
}

void SDL_helper::set_runflag(bool * flag)
{
	m_runflag = flag;
}

void SDL_helper::handle_events(SDL_Event * event)
{
	if (event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.sym == SDLK_LCTRL || event->key.keysym.sym == SDLK_RCTRL)
		{
			m_ctrl_down = true;
		}

		if (event->key.keysym.sym == SDLK_LSHIFT || event->key.keysym.sym == SDLK_RSHIFT)
		{
			m_shift_down = true;
		}
	}
	else if (event->type == SDL_KEYUP)
	{
		if (event->key.keysym.sym == SDLK_LCTRL || event->key.keysym.sym == SDLK_RCTRL)
		{
			m_ctrl_down = false;
		}

		if (event->key.keysym.sym == SDLK_LSHIFT || event->key.keysym.sym == SDLK_RSHIFT)
		{
			m_shift_down = false;
		}
	}
	else if (event->type == SDL_WINDOWEVENT)
	{
		if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		{
			SDL_GetWindowSize(m_sdl_window, &m_window_size.x, &m_window_size.y);
		}
	}
}
