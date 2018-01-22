#ifndef INPUT_HISTORY_HPP
#define INPUT_HISTORY_HPP

#include <obs-module.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include "input-source.hpp"
extern "C" {
#include "util.h"
}

#define MAX_HISTORY_SIZE 5

struct KeyBundle {
    bool m_empty = true;
    uint16_t m_keys[MAX_SIMULTANEOUS_KEYS] = { 0 };

    void merge(KeyBundle other);

    std::string to_string(bool fix, bool include_mouse);
    bool compare(KeyBundle* other);
    bool is_only_mouse();
};

struct InputHistorySource
{
    obs_source_t *m_source = nullptr;
    obs_source_t *m_text_source = nullptr;

    uint8_t m_history_size = 1;

    uint32_t cx = 0;
    uint32_t cy = 0;
    uint32_t m_update_interval = 1, m_counter = 0;

    bool m_fix_cutting = false;
    bool m_include_mouse = false;
    bool m_update_keys = false;
    bool m_dir_up = false;
    bool m_auto_clear = false;
    bool m_repeat_keys = false;

    KeyBundle m_current_keys;
    KeyBundle m_prev_keys;
    KeyBundle m_history[MAX_HISTORY_SIZE];

    float m_clear_timer = 0.f;
    int m_clear_interval = 0;

    inline InputHistorySource(obs_source_t *source_, obs_data_t *settings) :
        m_source(source_)
    {
        m_text_source = obs_source_create("text_gdiplus\0", "history-text-source", settings, NULL);
        obs_source_add_active_child(m_source, m_text_source);
        obs_source_update(m_text_source, settings);
        obs_source_update(m_source, settings);
    }

    inline ~InputHistorySource()
    {
        unload_textsource();
    }

    void unload_textsource(void);

    bool is_pressed(int k);
    void add_to_history(KeyBundle b);
    void clear_history(void);
    KeyBundle check_keys(void);

    inline void Update(obs_data_t *settings);
    inline void Tick(float seconds);
    inline void Render(gs_effect_t *effect);
};

// Util for registering the source
static bool clear_history(obs_properties_t *props, obs_property_t *property, void *data);

static obs_properties_t *get_properties_for_history(void *data);

void register_history();

#endif