/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "sdl_helper.hpp"
#include "localization.hpp"
#include "palette.hpp"

sdl_helper::sdl_helper()
{
}

sdl_helper::~sdl_helper()
{
    close();
    delete m_palette;
    delete m_font_helper;
    delete m_localization;

    m_localization = nullptr;
    m_font_helper = nullptr;
    m_sdl_renderer = nullptr;
    m_sdl_window = nullptr;
    m_palette = nullptr;
}

bool sdl_helper::init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        printf(SDL_INIT_FAILED, SDL_GetError());
        m_init_success = false;
    }

    m_sdl_window = SDL_CreateWindow(SDL_WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SDL_WINDOW_W,
                                    SDL_WINDOW_H, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (m_sdl_window == nullptr) {
        printf(SDL_CREATE_WINDOW_FAILED, SDL_GetError());
        m_init_success = false;
    } else {
        SDL_SetWindowMinimumSize(m_sdl_window, SDL_WINDOW_W, SDL_WINDOW_H);
        m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        /* Window icon */
        static const std::string windows = "Windows";
        if (windows != SDL_GetPlatform()) /* Windows gets the icon from the exe */
        {
            m_windows = false;
            const auto ico_surface = IMG_Load(PATH_WINDOW_ICON);

            if (ico_surface) {
                SDL_SetWindowIcon(m_sdl_window, ico_surface);
                SDL_FreeSurface(ico_surface);
            } else {
                printf(SDL_LOAD_WINDOW_ICON_FAILED, IMG_GetError());
                /* Non fatal */
            }
        }
    }

    if (m_sdl_renderer == nullptr) {
        printf(SDL_CREATE_RENDERER_FAILED, SDL_GetError());
        m_init_success = false;
    } else {
        SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
        SDL_GetWindowSize(m_sdl_window, &m_window_size.x, &m_window_size.y);
    }

    if (TTF_Init() == -1) {
        printf(SDL_TTF_INIT_FAILED, TTF_GetError());
        m_init_success = false;
    } else {
        m_large_font = TTF_OpenFont(PATH_UNICODE_FONT, FONT_LARGE);
        m_default_font = TTF_OpenFont(PATH_UNICODE_FONT, FONT_DEFAULT);

        if (!m_default_font) {
            printf(SDL_FONT_LOADING_FAILED);
            m_init_success = false;
        } else {
            m_font_helper = new font_helper(this);
            m_large_font_height = TTF_FontHeight(m_large_font);
            m_wstring_font_height = TTF_FontHeight(m_default_font);

            /* Cursors */
            m_size_h = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
            m_size_v = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
            m_move = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
            m_i_beam = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
            m_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

            m_have_cursors = m_size_h && m_size_v && m_move && m_i_beam && m_arrow;

            m_localization = new localization(PATH_TRANSLATIONS, this);
        }
    }

    m_frame_timer.start();

    m_palette = new palette();
    return m_init_success;
}

void sdl_helper::close()
{
    SDL_DestroyRenderer(m_sdl_renderer);
    SDL_DestroyWindow(m_sdl_window);

    if (m_default_font)
        TTF_CloseFont(m_default_font);
    if (m_large_font)
        TTF_CloseFont(m_large_font);

    m_large_font = nullptr;
    m_default_font = nullptr;

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

void sdl_helper::clear() const
{
    util_set_color(m_palette->dark_gray());
    SDL_RenderClear(m_sdl_renderer);
}

void sdl_helper::repaint() const
{
    SDL_RenderPresent(m_sdl_renderer);
}

void sdl_helper::util_set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const
{
    SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
}

void sdl_helper::util_set_color(const SDL_Color* color) const
{
    if (color == nullptr)
        util_set_color(m_palette->white()->r, m_palette->white()->g, m_palette->white()->b, m_palette->white()->a);
    else
        util_set_color(color->r, color->g, color->b, color->a);
}

void sdl_helper::util_draw_line(const int x1, const int y1, const int x2, const int y2, const SDL_Color* color) const
{
    util_set_color(color);
    SDL_RenderDrawLine(m_sdl_renderer, x1, y1, x2, y2);
}

void sdl_helper::util_draw_line(const SDL_Point &p1, const SDL_Point &p2, const SDL_Color* color) const
{
    util_set_color(color);
    SDL_RenderDrawLine(m_sdl_renderer, p1.x, p1.y, p2.x, p2.y);
}

void sdl_helper::util_draw_rect(const SDL_Rect* rect, const SDL_Color* color) const
{
    util_set_color(color);
    SDL_RenderDrawRect(m_sdl_renderer, rect);
}

void sdl_helper::util_draw_rect(const int x, const int y, const int w, const int h, const SDL_Color* color) const
{
    SDL_Rect temp_rect = {x, y, w, h};
    util_set_color(color);
    SDL_RenderDrawRect(m_sdl_renderer, &temp_rect);
}

void sdl_helper::util_fill_rect(const SDL_Rect* rect, const SDL_Color* color) const
{
    util_set_color(color);
    SDL_RenderFillRect(m_sdl_renderer, rect);
}

void sdl_helper::util_fill_rect_shadow(const SDL_Rect* rect, const SDL_Color* color, const int8_t offset) const
{
    SDL_Rect temp_rect = {rect->x + offset, rect->y + offset, rect->w, rect->h};
    util_fill_rect(&temp_rect, m_palette->black());
    util_fill_rect(rect, color);
}

void sdl_helper::util_fill_rect(const int x, const int y, const int w, const int h, const SDL_Color* color) const
{
    util_set_color(color);
    SDL_Rect temp_rect = SDL_Rect{x, y, w, h};
    SDL_RenderFillRect(m_sdl_renderer, &temp_rect);
}

void sdl_helper::util_fill_rect(const SDL_Rect* rect, const SDL_Color* color, const uint8_t alpha) const
{
    SDL_Color temp;
    temp.a = alpha;
    temp.r = color->r;
    temp.g = color->g;
    temp.b = color->b;
    util_fill_rect(rect, &temp);
}

uint8_t sdl_helper::util_default_text_height() const
{
    return m_wstring_font_height;
}

bool sdl_helper::util_is_in_rect(const SDL_Rect* rect, const int x, const int y)
{
    return x >= rect->x && x <= (rect->x + rect->w) && y >= rect->y && y <= (rect->y + rect->h);
}

bool sdl_helper::util_mouse_in_rect(const SDL_Rect* rect)
{
    return SDL_PointInRect(&m_mouse_pos, rect);
}

void sdl_helper::util_text(const std::string* text, const int x, const int y, const SDL_Color* color,
                           const uint8_t font) const
{
    const auto ttf_font = get_font(font);
    if (color == nullptr)
        m_font_helper->draw(text, x, y, ttf_font, m_palette->white());
    else
        m_font_helper->draw(text, x, y, ttf_font, color);
}

void
sdl_helper::util_text(const std::string* text, const int x, const int y, const SDL_Color* color, const uint8_t font,
                      const uint8_t scale) const
{
    const auto ttf_font = get_font(font);
    if (color == nullptr)
        m_font_helper->draw(text, x, y, ttf_font, m_palette->white(), scale);
    else
        m_font_helper->draw(text, x, y, ttf_font, color, scale);
}

void
sdl_helper::util_text_rot(const std::string* text, const int x, const int y, const SDL_Color* color, const double angle,
                          const uint8_t font) const
{
    const auto ttf_font = get_font(font);
    if (color == nullptr)
        m_font_helper->draw_rot(text, x, y, ttf_font, m_palette->white(), angle);
    else
        m_font_helper->draw_rot(text, x, y, ttf_font, color, angle);
}

SDL_Rect sdl_helper::util_text_dim(const std::string* text, const uint8_t font) const
{
    return m_font_helper->get_text_dimension(get_font(font), text);
}

SDL_Point* sdl_helper::util_window_size()
{
    return &m_window_size;
}

void sdl_helper::util_cut_string(std::string &s, const int max_width, const bool front) const
{
    auto w = util_text_dim(&s).w;
    auto was_cut = false;
    while (w > max_width) {
        if (front)
            s = s.substr(0, s.size() - 1);
        else
            s = s.substr(1, s.size());
        w = util_text_dim(&s).w;
        was_cut = true;
    }

    if (was_cut) {
        if (front)
            s.append("...");
        else
            s = "..." + s;
    }
}

void sdl_helper::util_open_url(std::string url)
{
    if (url.find("http://") != 0 && url.find("https://") != 0) {
        url = "https://" + url; /* Assume https ¯\_(ツ)_/¯ */
    }

    auto result = 0;
#ifdef _WIN32
    result = system(("start " + url).c_str());
#else
    result = system(("xdg-open " + url + "& disown").c_str());
#endif
    if (result < 0) {
        printf("Error while opening url(%s)\n", url.c_str());
    }
}

bool sdl_helper::util_check_texture_path(const char* path) const
{
    texture temp;
    const auto flag = temp.load(path, m_sdl_renderer);
    temp.free();
    return flag;
}

void sdl_helper::init_controllers()
{
    for (auto i = 0; i < SDL_NumJoysticks(); i++) {
        auto c = SDL_GameControllerOpen(i);
        if (c) {
            m_controllers.emplace_back(c);
        } else {
            printf(SDL_OPEN_CONTROLLER_FAILED, i, SDL_GetError());
        }
    }
}

void sdl_helper::set_cursor(const uint8_t type) const
{
    if (m_have_cursors) {
        switch (type) {
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
            case CURSOR_SPECIAL:;
        }
    }
}

bool sdl_helper::is_ctrl_down() const
{
    return m_ctrl_down;
}

bool sdl_helper::is_shift_down() const
{
    return m_shift_down;
}

void sdl_helper::exit_loop() const
{
    *m_runflag = false;
}

void sdl_helper::set_run_flag(bool* flag)
{
    m_runflag = flag;
}

void sdl_helper::handle_events(SDL_Event* event)
{
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_LCTRL || event->key.keysym.sym == SDLK_RCTRL) {
            m_ctrl_down = true;
        }

        if (event->key.keysym.sym == SDLK_LSHIFT || event->key.keysym.sym == SDLK_RSHIFT) {
            m_shift_down = true;
        }
    } else if (event->type == SDL_KEYUP) {
        if (event->key.keysym.sym == SDLK_LCTRL || event->key.keysym.sym == SDLK_RCTRL) {
            m_ctrl_down = false;
        }

        if (event->key.keysym.sym == SDLK_LSHIFT || event->key.keysym.sym == SDLK_RSHIFT) {
            m_shift_down = false;
        }
    } else if (event->type == SDL_WINDOWEVENT) {
        if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            SDL_GetWindowSize(m_sdl_window, &m_window_size.x, &m_window_size.y);
        }
    } else if (event->type == SDL_MOUSEMOTION) {
        m_mouse_pos = {event->motion.x, event->motion.y};
    }
}

uint8_t sdl_helper::util_font_height(const uint8_t font) const
{
    switch (font) {
        default:
        case FONT_WSTRING_LARGE:
            return m_large_font_height;
        case FONT_WSTRING:
            return m_wstring_font_height;
    }
}

bool sdl_helper::handle_controller_disconnect(const int32_t id)
{
    printf("id: %i\n", id);
    auto flag = false;
    if (id < 0)
        return false; /* Negative IDs shouldn't happen, I think */

    if (id < m_controllers.size()) {
        if (m_controllers[id]) {
            SDL_GameControllerClose(m_controllers[id]);
            m_controllers[id] = nullptr;
            flag = true;
        }
    } else if (!m_controllers.empty()) /* Just disconnect the last one :P who even uses multiple game pads */
    {
        SDL_GameControllerClose(m_controllers[m_controllers.size() - 1]);
        m_controllers[m_controllers.size() - 1] = nullptr;
        flag = true;
    }
    return flag;
}

bool sdl_helper::handle_controller_connect(const int32_t id)
{
    SDL_GameController* c;
    auto flag = false;
    if (id < 0)
        return false; /* Negative IDs shouldn't happen, I think */

    if (id < m_controllers.size()) {
        if (!m_controllers[id]) {
            c = SDL_GameControllerOpen(id);
            if (c) {
                m_controllers[id] = c;
                flag = true;
            } else {
                printf(SDL_OPEN_CONTROLLER_FAILED, id, SDL_GetError());
            }
        }
    } else {
        c = SDL_GameControllerOpen(id);
        if (c) {
            m_controllers.emplace_back(c);
            flag = true;
        } else {
            printf(SDL_OPEN_CONTROLLER_FAILED, id, SDL_GetError());
        }
    }
    return flag;
}

void sdl_helper::start_frame()
{
    m_frame_cap_timer.start();
}

void sdl_helper::end_frame()
{
    m_fps = m_counted_frames / (m_frame_timer.get_time() / 1000.f);
    m_counted_frames++;
    if (m_counted_frames + 1 >= UINT16_MAX) {
        m_counted_frames = 0;
        m_frame_timer.start();
    }
}

void sdl_helper::cap_frame() const
{
    if (m_frame_cap_timer.get_delta() < SDL_WINDOW_TPF)
        SDL_Delay(SDL_WINDOW_TPF - m_frame_cap_timer.get_time());
}

float sdl_helper::util_get_fps() const
{
    return m_fps;
}

std::wstring sdl_helper::util_utf8_to_wstring(const std::string &str)
{
#ifdef WINDOWS
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(str);
#else
    /* Conversion taken from
     * https://www.linuxquestions.org/questions/programming-9/wstring-utf8-conversion-in-pure-c-701084/
     **/
    std::wstring dest = L"";
    wchar_t w = 0;
    int bytes = 0;
    wchar_t err = L'�';

    for (size_t i = 0; i < str.size(); i++) {
        unsigned char c = (unsigned char) str[i];
        if (c <= 0x7f)
            // first byte
        {
            if (bytes) {
                dest.push_back(err);
                bytes = 0;
            }
            dest.push_back((wchar_t) c);
        } else if (c <= 0xbf)
            //second/third/etc byte
        {
            if (bytes) {
                w = ((w << 6) | (c & 0x3f));
                bytes--;
                if (bytes == 0)
                    dest.push_back(w);
            } else
                dest.push_back(err);
        } else if (c <= 0xdf)
            //2byte sequence start
        {
            bytes = 1;
            w = c & 0x1f;
        } else if (c <= 0xef)
            //3byte sequence start
        {
            bytes = 2;
            w = c & 0x0f;
        } else if (c <= 0xf7)
            //3byte sequence start
        {
            bytes = 3;
            w = c & 0x07;
        } else {
            dest.push_back(err);
            bytes = 0;
        }
    }
    if (bytes)
        dest.push_back(err);
    return dest;
#endif
}

std::string sdl_helper::util_wstring_to_utf8(const std::wstring &str)
{
#ifdef WINDOWS
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(str);
#else
    /* Conversion taken from
     * https://www.linuxquestions.org/questions/programming-9/wstring-utf8-conversion-in-pure-c-701084/
     **/
    std::string dest = "";
    for (size_t i = 0; i < str.size(); i++) {
        wchar_t w = str[i];
        if (w <= 0x7f) {
            dest.push_back((char) w);
        } else if (w <= 0x7ff) {
            dest.push_back(0xc0 | ((w >> 6) & 0x1f));
            dest.push_back(0x80 | (w & 0x3f));
        } else if (w <= 0xffff) {
            dest.push_back(0xe0 | ((w >> 12) & 0x0f));
            dest.push_back(0x80 | ((w >> 6) & 0x3f));
            dest.push_back(0x80 | (w & 0x3f));
        } else if (w <= 0x10ffff) {
            dest.push_back(0xf0 | ((w >> 18) & 0x07));
            dest.push_back(0x80 | ((w >> 12) & 0x3f));
            dest.push_back(0x80 | ((w >> 6) & 0x3f));
            dest.push_back(0x80 | (w & 0x3f));
        } else {
            dest.push_back('?');
        }
    }
    return dest;
#endif
}

void sdl_helper::format_text(const std::string* s, std::vector<std::unique_ptr<std::string>> &out, SDL_Rect &dim,
                             const uint8_t font) const
{
    if (!s || s->empty())
        return;
    const auto height = util_font_height(font);

    SDL_Rect temp = {};
    if (s->find(new_line) == std::string::npos) {
        out.push_back(std::make_unique<std::string>(*s));
        dim.h = height;
        temp = util_text_dim(s, font);
        dim.w = temp.w;
        return;
    }

    auto width = 0;
    auto lines = 1;

    auto start = 0U;
    auto end = s->find(new_line);
    std::string token;

    do {
        token = s->substr(start, end - start);
        out.push_back(std::make_unique<std::string>(token));
        start = end + new_line.length();

        if (!token.empty())
            temp = util_text_dim(&token, font);
        width = UTIL_MAX(temp.w, width);
        lines++;
    } while ((end = s->find(new_line, start)) != std::string::npos);

    token = s->substr(start, std::string::npos);
    out.push_back(std::make_unique<std::string>(token));

    if (!token.empty()) {
        temp = util_text_dim(&token, font);
        width = UTIL_MAX(temp.w, width);
    }

    dim.h = ((height + LINE_SPACE) * lines) - LINE_SPACE; // Last line space doesn't count
    dim.w = width;
}

std::string sdl_helper::loc(const char* id) const
{
    if (m_localization)
        return m_localization->localize(id);
    return std::string(id);
}


#include "../../../libuiohook/include/uiohook.h"
#include "../../../io-obs/util/util.hpp"

static uint32_t KEY_MAP[][2]{
        /* Alphabet */
        {VC_A,             SDLK_a},
        {VC_B,             SDLK_b},
        {VC_C,             SDLK_c},
        {VC_D,             SDLK_d},
        {VC_E,             SDLK_e},
        {VC_F,             SDLK_f},
        {VC_G,             SDLK_g},
        {VC_H,             SDLK_h},
        {VC_I,             SDLK_i},
        {VC_J,             SDLK_j},
        {VC_K,             SDLK_k},
        {VC_L,             SDLK_l},
        {VC_M,             SDLK_m},
        {VC_N,             SDLK_n},
        {VC_O,             SDLK_o},
        {VC_P,             SDLK_p},
        {VC_Q,             SDLK_q},
        {VC_R,             SDLK_r},
        {VC_S,             SDLK_s},
        {VC_T,             SDLK_t},
        {VC_U,             SDLK_u},
        {VC_V,             SDLK_v},
        {VC_W,             SDLK_w},
        {VC_X,             SDLK_x},
        {VC_Y,             SDLK_y},
        {VC_Z,             SDLK_z},

        /* Numbers */
        {VC_0,             SDLK_0},
        {VC_1,             SDLK_1},
        {VC_2,             SDLK_2},
        {VC_3,             SDLK_3},
        {VC_4,             SDLK_4},
        {VC_5,             SDLK_5},
        {VC_6,             SDLK_6},
        {VC_7,             SDLK_7},
        {VC_8,             SDLK_8},
        {VC_9,             SDLK_9},
        {VC_KP_0,          SDLK_KP_0},
        {VC_KP_1,          SDLK_KP_1},
        {VC_KP_2,          SDLK_KP_2},
        {VC_KP_3,          SDLK_KP_3},
        {VC_KP_4,          SDLK_KP_4},
        {VC_KP_5,          SDLK_KP_5},
        {VC_KP_6,          SDLK_KP_6},
        {VC_KP_7,          SDLK_KP_7},
        {VC_KP_8,          SDLK_KP_8},
        {VC_KP_9,          SDLK_KP_9},
        {VC_KP_ADD,        SDLK_KP_PLUS},
        {VC_KP_SEPARATOR,  SDLK_KP_COMMA},

        /* Numpad misc */
        {VC_KP_ADD,        SDLK_KP_PLUS},
        {VC_KP_SUBTRACT,   SDLK_KP_MINUS},
        {VC_KP_ENTER,      SDLK_KP_ENTER},
        {VC_KP_MULTIPLY,   SDLK_KP_MULTIPLY},
        {VC_KP_DIVIDE,     SDLK_KP_DIVIDE},
        {VC_NUM_LOCK,      SDLK_NUMLOCKCLEAR},
        {VC_KP_SEPARATOR,  SDLK_KP_COMMA},

        /* Function keys */
        {VC_F1,            SDLK_F1},
        {VC_F2,            SDLK_F2},
        {VC_F3,            SDLK_F3},
        {VC_F4,            SDLK_F4},
        {VC_F5,            SDLK_F5},
        {VC_F6,            SDLK_F6},
        {VC_F7,            SDLK_F7},
        {VC_F8,            SDLK_F8},
        {VC_F9,            SDLK_F9},
        {VC_F10,           SDLK_F10},
        {VC_F11,           SDLK_F11},
        {VC_F12,           SDLK_F12},
        {VC_F13,           SDLK_F13},
        {VC_F14,           SDLK_F14},
        {VC_F15,           SDLK_F15},
        {VC_F16,           SDLK_F16},
        {VC_F17,           SDLK_F17},
        {VC_F18,           SDLK_F18},
        {VC_F19,           SDLK_F19},
        {VC_F20,           SDLK_F20},
        {VC_F21,           SDLK_F21},
        {VC_F22,           SDLK_F22},
        {VC_F23,           SDLK_F23},
        {VC_F24,           SDLK_F24},

        /* Mask keys*/
        {VC_ALT_L,         SDLK_LALT},
        {VC_ALT_R,         SDLK_RALT},
        {VC_CONTROL_L,     SDLK_LCTRL},
        {VC_CONTROL_R,     SDLK_RCTRL},
        {VC_SHIFT_L,       SDLK_LSHIFT},
        {VC_SHIFT_R,       SDLK_RSHIFT},
        {VC_META_L,        SDLK_LGUI},
        {VC_META_R,        SDLK_RGUI},

        /* Misc */
        {VC_BACKSPACE,     SDLK_BACKSPACE},
        {VC_ENTER,         SDLK_RETURN},
        {VC_SPACE,         SDLK_SPACE},
        {VC_TAB,           SDLK_TAB},
        {VC_ESCAPE,        SDLK_ESCAPE},
        /* libuiohook mixes up keypad arrow keys with normal arrow keys */
        {VC_KP_UP,         SDLK_UP},
        {VC_KP_DOWN,       SDLK_DOWN},
        {VC_KP_LEFT,       SDLK_LEFT},
        {VC_KP_RIGHT,      SDLK_RIGHT},
        {VC_DELETE,        SDLK_DELETE},
        {VC_INSERT,        SDLK_INSERT},
        {VC_HOME,          SDLK_HOME},
        {VC_PAGE_UP,       SDLK_PAGEUP},
        {VC_PAGE_DOWN,     SDLK_PAGEDOWN},
        {VC_END,           SDLK_END},
        {VC_PRINTSCREEN,   SDLK_PRINTSCREEN},
        {VC_SCROLL_LOCK,   SDLK_SCROLLLOCK},
        {VC_PAUSE,         SDLK_PAUSE},
        {VC_MINUS,         SDLK_MINUS},
        {VC_EQUALS,        SDLK_EQUALS},
        {VC_CAPS_LOCK,     SDLK_CAPSLOCK},
        {VC_OPEN_BRACKET,  SDLK_LEFTBRACKET},
        {VC_CLOSE_BRACKET, SDLK_RIGHTBRACKET},
        {VC_BACK_SLASH,    SDLK_BACKSLASH},
        {VC_SLASH,         SDLK_SLASH},
        {VC_COMMA,         SDLK_COMMA},
        {VC_PERIOD,        SDLK_PERIOD},
        {VC_SEMICOLON,     SDLK_SEMICOLON},
        {VC_QUOTE,         SDLK_QUOTE},
        {VC_BACKQUOTE,     SDLK_BACKQUOTE},

        /* Game pad */
        {PAD_TO_VC(PAD_A),          TO_PAD_MASK(SDL_CONTROLLER_BUTTON_A)},
        {PAD_TO_VC(PAD_B),          TO_PAD_MASK(SDL_CONTROLLER_BUTTON_B)},
        {PAD_TO_VC(PAD_X),          TO_PAD_MASK(SDL_CONTROLLER_BUTTON_X)},
        {PAD_TO_VC(PAD_Y),          TO_PAD_MASK(SDL_CONTROLLER_BUTTON_Y)},
        {PAD_TO_VC(PAD_LB),         TO_PAD_MASK(SDL_CONTROLLER_BUTTON_LEFTSHOULDER)},
        {PAD_TO_VC(PAD_RB),         TO_PAD_MASK(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)},
        {PAD_TO_VC(PAD_BACK),       TO_PAD_MASK(SDL_CONTROLLER_BUTTON_BACK)},
        {PAD_TO_VC(PAD_START),      TO_PAD_MASK(SDL_CONTROLLER_BUTTON_START)},
        {PAD_TO_VC(PAD_DPAD_DOWN),  TO_PAD_MASK(SDL_CONTROLLER_BUTTON_DPAD_DOWN)},
        {PAD_TO_VC(PAD_DPAD_UP),    TO_PAD_MASK(SDL_CONTROLLER_BUTTON_DPAD_UP)},
        {PAD_TO_VC(PAD_DPAD_LEFT),  TO_PAD_MASK(SDL_CONTROLLER_BUTTON_DPAD_LEFT)},
        {PAD_TO_VC(PAD_DPAD_RIGHT), TO_PAD_MASK(SDL_CONTROLLER_BUTTON_DPAD_RIGHT)},
        {PAD_TO_VC(PAD_L_ANALOG),   TO_PAD_MASK(SDL_CONTROLLER_BUTTON_LEFTSTICK)},
        {PAD_TO_VC(PAD_R_ANALOG),   TO_PAD_MASK(SDL_CONTROLLER_BUTTON_RIGHTSTICK)},
        {PAD_TO_VC(PAD_X_BOX_KEY),  TO_PAD_MASK(SDL_CONTROLLER_BUTTON_GUIDE)},
        {PAD_TO_VC(PAD_START),      TO_PAD_MASK(SDL_CONTROLLER_BUTTON_START)},

        /* Mouse */
        {VC_MOUSE_BUTTON1,          TO_MOUSE_MASK(SDL_BUTTON_LEFT)},
        {VC_MOUSE_BUTTON2,          TO_MOUSE_MASK(SDL_BUTTON_RIGHT)},
        {VC_MOUSE_BUTTON3,          TO_MOUSE_MASK(SDL_BUTTON_MIDDLE)},
        {VC_MOUSE_BUTTON4,          TO_MOUSE_MASK(SDL_BUTTON_X1)},
        {VC_MOUSE_BUTTON5,          TO_MOUSE_MASK(SDL_BUTTON_X2)}};

uint32_t sdl_helper::vc_to_sdl_key(const uint16_t key)
{
    for (auto &i : KEY_MAP) {
        if (i[0] == key)
            return i[1];
    }
    return SDLK_UNKNOWN;
}

uint16_t sdl_helper::sdl_key_to_vc(const uint32_t key)
{
    for (auto &i : KEY_MAP) {
        if (i[1] == key)
            return i[0];
    }
    return VC_UNDEFINED;
}
