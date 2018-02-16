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
#include "../ccl/ccl.hpp"
#include "../util/layouttype.hpp"
#include "../util/util.hpp"

extern "C" {
#include <graphics/image-file.h>
}

/* Platform dependend gamepad implementation */
#if HAVE_XINPUT
#include "../gamepad/windows-gamepad.hpp"
#endif

#if LINUX_INPUT
#include "../gamepad/linux-gamepad.hpp"
#endif

#include "../hook/hook-helper.hpp"

struct InputSource
{
    obs_source_t *m_source = nullptr;
    gs_image_file_t *m_image = nullptr;
    uint32_t cx = 0;
    uint32_t cy = 0;

    bool m_is_controller = false;
    bool m_monitor_use_center = false;
    int32_t m_monitor_h = 0, m_monitor_v = 0;
    uint8_t m_mouse_dead_zone = 0;

    float m_old_angle = 0.f;

    std::string m_image_file;
    std::string m_layout_file;

    OverlayLayout m_layout;

#ifdef HAVE_XINPUT
    WindowsGamepad * m_gamepad = nullptr;
#endif

#ifdef LINUX_INPUT
    LinuxGamepad * m_gamepad = nullptr;
#endif

    inline InputSource(obs_source_t *source_, obs_data_t *settings) :
        m_source(source_)
    {
        obs_source_update(m_source, settings);
    }

    inline ~InputSource()
    {
        unload_texture();
        unload_layout();
        
        if (m_gamepad)
        {
            delete m_gamepad;
            m_gamepad = nullptr;
        }
    }

    void load_texture(void);
    void load_layout(void);
    void unload_layout(void);
    void unload_texture(void);

    void draw_key(gs_effect_t * effect, InputKey * key, uint16_t x, uint16_t y, bool rot, float angle);
    void draw_key(gs_effect_t * effect, InputKey * key, uint16_t x, uint16_t y);
    void draw_key(gs_effect_t * effect, InputKey * key);

    void check_keys(void);

    inline void Update(obs_data_t *settings);
    inline void Tick(float seconds);
    inline void Render(gs_effect_t *effect);
};

static bool is_controller_changed(obs_properties_t * props, obs_property_t *p, obs_data_t * s);

static bool is_mouse_changed(obs_properties_t * props, obs_property_t * p, obs_data_t * s);

static bool use_monitor_center_changed(obs_properties_t * props, obs_property_t *p, obs_data_t * s);

// For registering
static obs_properties_t *get_properties_for_overlay(void *data);

void register_overlay_source();

#endif