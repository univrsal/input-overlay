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
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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
	else {
		SDL_SetWindowMinimumSize(m_sdl_window, WINDOW_WIDTH, WINDOW_HEIGHT);
		m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	}

	if (m_sdl_renderer == NULL) {
		printf("Creating SDL Renderer failed! Error: %s\n", SDL_GetError());
		m_init_success = false;
	}

	if (TTF_Init() == -1) {
		printf("Initializing SDL_ttf failed! Error: %s\n", TTF_GetError());
		m_init_success = false;
	}
	else {

		m_default_font = TTF_OpenFont("./roboto.ttf", FONT_DEFAULT);
		m_font_helper = new FontHelper(this);
	}

	m_palette = new Palette();
	return m_init_success;
}

void SDL_helper::close()
{
	SDL_DestroyRenderer(m_sdl_renderer);
	SDL_DestroyWindow(m_sdl_window);

	TTF_CloseFont(m_default_font);

	TTF_Quit();
	SDL_Quit();
}

void SDL_helper::clear()
{
	util_set_color(m_palette->black());
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
	SDL_Color temp_color = { 0, 0, 0, 255 };
	util_fill_rect(&temp_rect, &temp_color);
	util_fill_rect(rect, color);
}

void SDL_helper::util_fill_rect(int x, int y, int w, int h, const SDL_Color * color)
{
	util_set_color(color);
	SDL_Rect temp_rect = SDL_Rect{ x, y, w, h };
	SDL_RenderFillRect(m_sdl_renderer, &temp_rect);
}

void SDL_helper::util_text(std::string * text, int x, int y, const SDL_Color * color)
{
	if (color == NULL)
		m_font_helper->draw(text, x, y, m_default_font, m_palette->white());
	else
		m_font_helper->draw(text, x, y, m_default_font, color);
}

SDL_Rect SDL_helper::util_text_dim(std::string * text)
{
	return m_font_helper->get_text_dimension(m_default_font, text);
}
