#ifndef INPUT_SOURCE_HPP
#define INPUT_SOURCE_HPP

#include <obs-module.h>
#include <string>
#include <Windows.h>
#include <algorithm>
#include <clocale>
#include <locale>
#include <uiohook.h>
#include <list>
#include <stdarg.h>
#include "ccl/ccl.hpp"
#include "layouttype.hpp"
extern "C" {
#include <graphics\image-file.h>
#include "util.h"
}
#if HAVE_XINPUT
#include <Xinput.h>
#endif

#ifdef WINDOWS
#include <Windows.h>
#else
#if defined UNIX

#endif
#endif

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
    uint8_t texture_w;
    uint16_t texture_v_space;
    bool m_is_loaded = false;
    std::vector<InputKey> m_keys;

    uint16_t m_analog_stick_radius;
};

struct InputSource
{
    obs_source_t *m_source = nullptr;
    uint32_t cx = 0;
    uint32_t cy = 0;

#if HAVE_XINPUT
    XINPUT_STATE m_xinput;
#endif
    bool m_valid_controller = false;
    std::string m_image_file;
    std::wstring m_layout_file;
    gs_image_file_t *m_image = nullptr;
    OverlayLayout m_layout;
    GamepadSettings m_pad_settings;

    inline InputSource(obs_source_t *source_, obs_data_t *settings) :
        m_source(source_)
    {
        obs_source_update(m_source, settings);
    }

    inline ~InputSource()
    {
        UnloadOverlayTexure();
        UnloadOverlayLayout();
    }

    void LoadOverlayTexture(void);
    void LoadOverlayLayout(void);
    void UnloadOverlayLayout(void);
    void UnloadOverlayTexure(void);

    void DrawKey(gs_effect_t *effect, InputKey* key, uint16_t x, uint16_t y);
    void DrawKey(gs_effect_t *effect, InputKey* key);

    void CheckKeys(void);
    void CheckGamePadKeys(void);

    inline bool io_begins(const std::string line, const std::string prefix)
    {
        if (prefix.size() > line.size()) return false;
        return equal(prefix.begin(), prefix.end(), line.begin());
    }

    inline void io_cut(std::string& line)
    {
        line = line.substr(line.rfind('=') + 1, line.size());
    }

    // Read key order from a string e.g. "A,B,C,0x10"
    uint16_t read_chain(std::string& l)
    {
        std::string temp = l.substr(0, l.find(','));
        l = l.substr(l.find(',') + 1, l.size());

        if (temp.find("0x") != std::string::npos)
        {
            return std::stoul(temp, nullptr, 16);
        }
        else
        {
            return (uint16_t) 0x0;
        }
    }

    uint16_t read_chain_int(std::string& l)
    {
        std::string temp = l.substr(0, l.find(','));
        l = l.substr(l.find(',') + 1, l.size());
        return std::atoi(temp.c_str());
    }

    inline void Update(obs_data_t *settings);
    inline void Tick(float seconds);
    inline void Render(gs_effect_t *effect);
};

static bool is_controller_changed(obs_properties_t *props, obs_property_t *p, obs_data_t *s);

// For registering
static obs_properties_t *get_properties_for_overlay(void *data);

void register_overlay_source();

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