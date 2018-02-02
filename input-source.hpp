#ifndef INPUT_SOURCE_HPP
#define INPUT_SOURCE_HPP

/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

#include <obs-module.h>
#include <string>
#include <algorithm>
#include <clocale>
#include <locale>
#include <uiohook.h>
#include <list>
#include <stdarg.h>
#include <list>
#include <util/platform.h>
#include "ccl/ccl.hpp"
#include "layouttype.hpp"
#include "util.hpp"
extern "C" {
#include <graphics/image-file.h>
}

#if HAVE_XINPUT
#include <Xinput.h>
#endif

#define MAX_SIMULTANEOUS_KEYS 14
extern uint16_t pressed_keys[MAX_SIMULTANEOUS_KEYS];
extern int16_t mouse_x, mouse_y, mouse_x_smooth, mouse_y_smooth, mouse_last_x, mouse_last_y;
extern bool hook_initialized;

void util_clear_pressed(void);
bool util_pressed_empty(void);
bool util_key_exists(uint16_t vc);
void util_add_pressed(uint16_t vc);
void util_remove_pressed(uint16_t vc);

struct InputKey {
    uint16_t m_key_code;
    bool m_pressed = false;
    uint16_t texture_u, texture_v;
    uint16_t w, h;

    uint16_t x_offset; // used to center buttons that span over multiple columns
    uint16_t row, column; // On screen location (in pixels for mouse/controller layouts)
};

struct GamepadSettings {
    uint16_t m_analog_radius = 0;
    short m_controller_id = 0;
    
    float m_left_analog_x = 0.f, m_left_analog_y = 0.f;
    float m_right_analog_x = 0.f, m_right_analog_y = 0.f;
    
    uint16_t m_left_dead_zone = 0;
    uint16_t m_right_dead_zone = 0;
};

struct OverlayLayout {
    LayoutType m_type;
    uint8_t m_key_count;
    int8_t m_key_space_v, m_key_space_h;
    uint8_t m_btn_w, m_btn_h;
    uint16_t m_w, m_h;
    uint8_t m_rows, m_cols;
    uint16_t texture_v_space;
    bool m_is_loaded = false;
    bool m_mouse_movement = false;
    bool m_use_arrow = false;
    std::vector<InputKey> m_keys;

    uint16_t m_track_radius;
    uint16_t m_max_mouse_movement;
    float m_arrow_rot = 0.f;
};

struct InputSource
{
    obs_source_t *m_source = nullptr;
    gs_image_file_t *m_image = nullptr;
    uint32_t cx = 0;
    uint32_t cy = 0;

#if HAVE_XINPUT
    XINPUT_STATE m_xinput;
#endif
    bool m_valid_controller = false;
    int16_t m_last_d_x = 0, m_last_d_y = 0;

    std::string m_image_file;
    std::string m_layout_file;

    OverlayLayout m_layout;
    GamepadSettings m_pad_settings;

    inline InputSource(obs_source_t *source_, obs_data_t *settings) :
        m_source(source_)
    {
        obs_source_update(m_source, settings);
    }

    inline ~InputSource()
    {
        unload_texture();
        unload_layout();
    }

    void load_texture(void);
    void load_layout(void);
    void unload_layout(void);
    void unload_texture(void);

    void draw_key(gs_effect_t * effect, InputKey * key, uint16_t x, uint16_t y, bool rot, float angle);
    void draw_key(gs_effect_t * effect, InputKey * key, uint16_t x, uint16_t y);
    void draw_key(gs_effect_t * effect, InputKey * key);

    void check_keys(void);
    void check_gamepad(void);

    inline void Update(obs_data_t *settings);
    inline void Tick(float seconds);
    inline void Render(gs_effect_t *effect);
};

static bool is_controller_changed(obs_properties_t *props, obs_property_t *p, obs_data_t *s);

// For registering
static obs_properties_t *get_properties_for_overlay(void *data);

void register_overlay_source();

uint16_t mouse_to_vc(int m);
// libuiohook

#ifdef WINDOWS
DWORD WINAPI hook_thread_proc(LPVOID arg);
#else
void *hook_thread_proc(void *arg);
#endif

void dispatch_proc(uiohook_event * const event);

bool logger_proc(unsigned int level, const char *format, ...);

void start_hook(void);

void end_hook(void);

int hook_enable(void);

void proccess_event(uiohook_event * const event);

#endif