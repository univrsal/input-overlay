#ifndef INPUT_HISTORY_HPP
#define INPUT_HISTORY_HPP

#include <obs-module.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include <map>
#include "input-source.hpp"
#include "ccl/ccl.hpp"
#include "util.hpp"

extern "C" {
#include <graphics/image-file.h>
}

#define MAX_HISTORY_SIZE 5

/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

struct KeyNames {
    void load_from_file(std::string path);
    const char * get_name(uint16_t vc);

    ~KeyNames();

private:    
    std::map<uint16_t, std::string> m_names;
};

struct KeyBundle {
    bool m_empty = true;
    uint16_t m_keys[MAX_SIMULTANEOUS_KEYS] = { 0 };

    void merge(KeyBundle other);

    std::string to_string(bool fix, bool include_mouse, bool use_translation, bool use_fallback, KeyNames* names);
    bool compare(KeyBundle* other);
    bool is_only_mouse();
};

struct KeyIcon {
    uint16_t u, v;
};

enum IconDirection {
    DIR_DOWN,
    DIR_UP,
    DIR_LEFT,
    DIR_RIGHT
};

struct KeyIcons {
    ~KeyIcons();

    void load_from_file(std::string img_path, std::string cfg_path);
    KeyIcon * get_icon_for_key(uint16_t vc);

    uint16_t get_w(void) { return m_icon_w; }
    
    uint16_t get_h(void) { return m_icon_h; }
    bool is_loaded() { return m_loaded; }
    bool has_texture_for_bundle(KeyBundle * bundle);
    
    gs_image_file_t* get_texture(void) { return m_icon_texture; }

private:
    bool m_loaded = false;
    uint16_t m_icon_count, m_icon_w, m_icon_h;
    std::map<uint16_t, KeyIcon> m_icons;
    void unload_texture();
    gs_image_file_t *m_icon_texture = nullptr;
};

struct InputHistorySource
{
    obs_source_t *m_source = nullptr;
    obs_source_t *m_text_source = nullptr;

    uint8_t m_history_size = 1;

    uint32_t cx = 0;
    uint32_t cy = 0;
    uint32_t m_update_interval = 1, m_counter = 0;
    int16_t m_icon_v_space = 0, m_icon_h_space = 0;

    bool m_text_mode = true;
    bool m_fix_cutting = false;
    bool m_include_mouse = false;
    bool m_update_keys = false;
    bool m_auto_clear = false;
    bool m_repeat_keys = false;
    bool m_use_translation = false;
    bool m_use_fallback = false;
    
    IconDirection m_history_direction = DIR_DOWN;

    KeyBundle m_current_keys;
    KeyBundle m_prev_keys;
    KeyBundle m_history[MAX_HISTORY_SIZE];

    std::string m_key_name_path;
    std::string m_key_icon_path;
    std::string m_key_icon_config_path;
    KeyNames * m_key_names = nullptr;
    KeyIcons * m_key_icons = nullptr;

    float m_clear_timer = 0.f;
    int m_clear_interval = 0;

    inline InputHistorySource(obs_source_t *source_, obs_data_t *settings) :
        m_source(source_)
    {
        load_text_source();
    }

    inline ~InputHistorySource()
    {
        unload_icons();
        unload_translation();
        unload_text_source();
    }

    void load_text_source(void);
    void load_icons(void);
    void load_translation(void);
    
    void unload_text_source(void);
    void unload_icons(void);
    void unload_translation(void);

    void add_to_history(KeyBundle b);
    void clear_history(void);
    KeyBundle check_keys(void);
    void handle_text_history(void);
    void handle_icon_history(gs_effect_t * effect);

    inline void Update(obs_data_t *settings);
    inline void Tick(float seconds);
    inline void Render(gs_effect_t *effect);
};

// Util for registering the source
static bool clear_history(obs_properties_t *props, obs_property_t *property, void *data);

static bool mode_changed(obs_properties_t *props, obs_property_t *p, obs_data_t *s);

static obs_properties_t *get_properties_for_history(void *data);

void register_history();

#endif