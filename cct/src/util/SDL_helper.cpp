/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
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
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Initialization of SDL failed! Error: %s\n", SDL_GetError());
		m_init_success = false;
	}

	m_sdl_window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (m_sdl_window == NULL)
	{
		printf("Creating SDL Window failed! Error: %s\n", SDL_GetError());
		m_init_success = false;
	}
	else
	{
		SDL_SetWindowMinimumSize(m_sdl_window, WINDOW_WIDTH, WINDOW_HEIGHT);
		m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	}

	if (m_sdl_renderer == NULL)
	{
		printf("Creating SDL Renderer failed! Error: %s\n", SDL_GetError());
		m_init_success = false;
	}
	else
	{
		SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
		SDL_GetWindowSize(m_sdl_window, &m_window_size.x, &m_window_size.y);
	}

	if (TTF_Init() == -1)
	{
		printf("Initializing SDL_ttf failed! Error: %s\n", TTF_GetError());
		m_init_success = false;
	}
	else
	{

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

		m_default_font_height = TTF_FontHeight(m_default_font);
		m_wstr_font_height = TTF_FontHeight(m_utf8_font);

		/* Cursors */
		m_size_h = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
		m_size_v = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
		m_move = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		m_i_beam = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
		m_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

		m_have_cursors = m_size_h && m_size_v && m_move && m_i_beam && m_arrow;
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


	if (m_size_v)
		SDL_FreeCursor(m_size_v);

	if (m_size_h)
		SDL_FreeCursor(m_size_h);

	if (m_move)
		SDL_FreeCursor(m_move);

	if (m_i_beam)
		SDL_FreeCursor(m_i_beam);

	if (m_arrow)
		SDL_FreeCursor(m_arrow);

	m_size_v = nullptr;
	m_size_h = nullptr;
	m_move = nullptr;
	m_i_beam = nullptr;
	m_arrow = nullptr;
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

void SDL_helper::util_text_wstr(std::string * text, int x, int y, const SDL_Color * color)
{
	if (color == NULL)
		m_font_helper->draw(text, x, y, m_utf8_font, m_palette->white());
	else
		m_font_helper->draw(text, x, y, m_utf8_font, color);
}

SDL_Rect SDL_helper::util_text_wstr_dim(std::string * text)
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

inline void SDL_helper::util_enable_mask(uint16_t & masks, uint16_t mask)
{
	masks |= mask;
}

inline void SDL_helper::util_disable_mask(uint16_t & masks, uint16_t mask)
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

void SDL_helper::set_cursor(uint8_t type)
{
	if (m_have_cursors)
	{
		switch (type)
		{
		default:
		case CURSOR_ARROW:
			SDL_SetCursor(m_arrow);
			break;
		case CURSOR_I_BEAM:
			SDL_SetCursor(m_i_beam);
			break;
		case CURSOR_SIZE_ALL:
			SDL_SetCursor(m_move);
			break;
		case CURSOR_SIZE_H:
			SDL_SetCursor(m_size_h);
			break;
		case CURSOR_SIZE_V:
			SDL_SetCursor(m_size_v);
			break;
		case CURSOR_SPECIAL:
			break; /* NO-OP */
		}
	}
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

std::wstring SDL_helper::util_utf8_to_wstring(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(str);
}

std::string SDL_helper::util_wstring_to_utf8(const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str);
}

#include "../../../libuiohook/include/uiohook.h"
#define CCT /* Prevents util.hpp from including external headers */
#include "../../../util/util.hpp"
static uint32_t KEY_MAP[][2]
{
	/* Alphabet */
	{ VC_A, SDLK_a },{ VC_B, SDLK_b },{ VC_C, SDLK_c },{ VC_D, SDLK_d },{ VC_E, SDLK_e },{ VC_F, SDLK_f },{ VC_G, SDLK_g },{ VC_H, SDLK_h },{ VC_I, SDLK_i },
	{ VC_J, SDLK_j },{ VC_K, SDLK_k },{ VC_L, SDLK_l },{ VC_M, SDLK_m },{ VC_N, SDLK_n },{ VC_O, SDLK_o },{ VC_P, SDLK_p },{ VC_Q, SDLK_q },{ VC_R, SDLK_r },
	{ VC_S, SDLK_s },{ VC_T, SDLK_t },{ VC_U, SDLK_u },{ VC_V, SDLK_v },{ VC_X, SDLK_x },{ VC_Y, SDLK_y },
	/* Numbers */
	{ VC_0, SDLK_0 },{ VC_1, SDLK_1 },{ VC_2, SDLK_2 },{ VC_3, SDLK_3 },{ VC_4, SDLK_4 },{ VC_5, SDLK_5 },{ VC_6, SDLK_7 },{ VC_8, SDLK_9 },
	{ VC_KP_0, SDLK_KP_0 },{ VC_KP_1, SDLK_KP_1 },{ VC_KP_2, SDLK_KP_3 },{ VC_KP_3, SDLK_KP_3 },{ VC_KP_4, SDLK_KP_4 },{ VC_KP_5, SDLK_KP_5 },
	{ VC_KP_6, SDLK_KP_6 },{ VC_KP_7, SDLK_KP_7 },{ VC_KP_8, SDLK_KP_8 },{ VC_KP_9, SDLK_KP_9 },
	/* Numpad misc*/
	{ VC_KP_ADD, SDLK_KP_PLUS },{ VC_KP_SUBTRACT, SDLK_KP_MINUS },{ VC_KP_ENTER, SDLK_KP_ENTER },{ VC_KP_MULTIPLY, SDLK_KP_MULTIPLY },{ VC_KP_DIVIDE, SDLK_KP_DIVIDE },
	{ VC_NUM_LOCK, SDLK_NUMLOCKCLEAR },{ VC_KP_SEPARATOR, SDLK_KP_COMMA },
	/* Function keys */
	{ VC_F1, SDLK_F1 },{ VC_F2, SDLK_F2 },{ VC_F3, SDLK_F3 },{ VC_F4, SDLK_F4 },{ VC_F5, SDLK_F5 },{ VC_F6, SDLK_F6 },{ VC_F7, SDLK_F7 },{ VC_F8, SDLK_F8 },
	{ VC_F9, SDLK_F9 },{ VC_F10, SDLK_F10 },{ VC_F11, SDLK_F11 },{ VC_F12, SDLK_F12 },{ VC_F13, SDLK_F13 },{ VC_F14, SDLK_F14 },{ VC_F15, SDLK_F15 },
	{ VC_F16, SDLK_F16 },{ VC_F17, SDLK_F17 },{ VC_F18, SDLK_F18 },{ VC_F19, SDLK_F19 },{ VC_F20, SDLK_F20 },{ VC_F21, SDLK_F21 },{ VC_F22, SDLK_F22 },
	{ VC_F23, SDLK_F23 },{ VC_F24, SDLK_F24 },
	/* Mask keys*/
	{ VC_ALT_L, SDLK_LALT },{ VC_ALT_R, SDLK_RALT },{ VC_CONTROL_L, SDLK_LCTRL },{ VC_CONTROL_R, SDLK_RCTRL },{ VC_SHIFT_L, SDLK_LSHIFT },{ VC_SHIFT_R, SDLK_RSHIFT },
	{ VC_META_L, SDLK_LGUI },{ VC_META_R, SDLK_RGUI },
	/* Misc*/
	{ VC_BACKSPACE, SDLK_BACKSPACE },{ VC_ENTER, SDLK_RETURN },{ VC_SPACE, SDLK_SPACE },{ VC_TAB, SDLK_TAB },{ VC_ESCAPE, SDLK_ESCAPE },
	{ VC_UP, SDLK_UP },{ VC_DOWN, SDLK_DOWN },{ VC_LEFT, SDLK_LEFT },
	{ VC_DELETE, SDLK_DELETE },{ VC_INSERT, SDLK_INSERT },{ VC_HOME, SDLK_HOME },{ VC_PAGE_UP, SDLK_PAGEUP },{ VC_PAGE_DOWN, SDLK_PAGEDOWN },{ VC_END, SDLK_END },
	{ VC_PRINTSCREEN, SDLK_PRINTSCREEN },{ VC_SCROLL_LOCK, SDLK_SCROLLLOCK },{ VC_PAUSE, SDLK_PAUSE },
	/* Gamepad */
	{ PAD_TO_VC(PAD_A), SDL_CONTROLLER_BUTTON_A},{ PAD_TO_VC(PAD_B), SDL_CONTROLLER_BUTTON_B },{ PAD_TO_VC(PAD_X), SDL_CONTROLLER_BUTTON_X },
	{ PAD_TO_VC(PAD_LB), SDL_CONTROLLER_BUTTON_LEFTSHOULDER },{ PAD_TO_VC(PAD_RB), SDL_CONTROLLER_BUTTON_RIGHTSHOULDER },
	/*{ PAD_TO_VC(PAD_LT), SDL_CONTROLLER_AXIS_TRIGGERLEFT },{ PAD_TO_VC(PAD_RT), SDL_CONTROLLER_AXIS_TRIGGERRIGHT },*/
	{ PAD_TO_VC(PAD_BACK), SDL_CONTROLLER_BUTTON_BACK },{ PAD_TO_VC(PAD_START), SDL_CONTROLLER_BUTTON_START },
	{ PAD_TO_VC(PAD_DPAD_DOWN), SDL_CONTROLLER_BUTTON_DPAD_DOWN },{ PAD_TO_VC(PAD_DPAD_UP), SDL_CONTROLLER_BUTTON_DPAD_UP },
	{ PAD_TO_VC(PAD_DPAD_LEFT), SDL_CONTROLLER_BUTTON_DPAD_LEFT },{ PAD_TO_VC(PAD_DPAD_RIGHT), SDL_CONTROLLER_BUTTON_DPAD_RIGHT },
	{ PAD_TO_VC(PAD_L_ANALOG), SDL_CONTROLLER_BUTTON_LEFTSTICK },{ PAD_TO_VC(PAD_R_ANALOG), SDL_CONTROLLER_BUTTON_RIGHTSTICK },
	{ PAD_TO_VC(PAD_X_BOX_KEY), SDL_CONTROLLER_BUTTON_GUIDE },{ PAD_TO_VC(PAD_START), SDL_CONTROLLER_BUTTON_START },
	/**/
};
#define KEY_MAP_SIZE 113

uint32_t SDL_helper::vc_to_sdl_key(uint16_t key)
{
	for (int i = 0; i < 113; ++i)
	{
		if (KEY_MAP[i][0] == key)
			return KEY_MAP[i][1];
	}
	return SDLK_UNKNOWN;
}

uint16_t SDL_helper::sdl_key_to_vc(uint32_t key)
{
	for (int i = 0; i < 113; ++i)
	{
		if (KEY_MAP[i][1] == key)
			return KEY_MAP[i][0];
	}
	return VC_UNDEFINED;
}
