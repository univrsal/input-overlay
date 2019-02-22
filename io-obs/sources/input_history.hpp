/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <obs-module.h>
#include "../util/util.hpp"
#include "../util/layout_constants.hpp"
#include "../hook/gamepad_hook.hpp"
#include "../hook/hook_helper.hpp"

class input_queue;

extern "C" {
#include <graphics/image-file.h>
}

class history_icons;
class key_names;

#define MAX_HISTORY_SIZE 32
#define SET_FLAG(a, b)      (util_set_mask(m_settings.flags, a, b))
#define GET_FLAG(a)         (m_settings.flags & a)

namespace sources
{
    enum history_mode
    {
        MODE_TEXT,
        MODE_ICONS,
        MODE_COMMANDS
    };

    enum history_flags
    {
        FLAG_FIX_CUTTING    = 1 << 0,   /* Append space at the end of text to fix cursive fonts cutting off*/
        FLAG_INCLUDE_MOUSE  = 1 << 1,   /* Include mouse clicks and scrolling */
        FLAG_AUTO_CLEAR     = 1 << 2,   /* Use automatic clearing */
        FLAG_REPEAT_KEYS    = 1 << 3,   /* Allow repeated keys*/
        FLAG_MERGE_REPEAT   = 1 << 4,   /* Merge repeated keys into one entry */
        FLAG_CUSTOM_NAMES   = 1 << 5,   /* Use custom key name config */
        FLAG_USE_FALLBACK   = 1 << 6,   /* Use hardcoded names if config doesn't define a key name */
        FLAG_INCLUDE_PAD    = 1 << 7,   /* Include gamepad inputs */
        FLAG_USE_MASKS      = 1 << 8    /* Use event masks (Holding Ctrl, Shift etc.) */
    };

    struct history_settings
    {
        /* Configurable properties */
        history_mode mode = MODE_TEXT;          /* Mode for visualization */
        uint8_t history_size = 0;               /* Maximum amount of entries in history */
        uint8_t target_gamepad = 0;             /* Only one gamepad is used per source */
        uint16_t v_space = 0, h_space = 0;      /* Vertical/Horizontal space. h_space only for icons */
        history_direction dir = DIR_DOWN;       /* Flow direction of input display */
        double update_interval = 0.f;           /* Timespan in which inputs will be accumulated */
        double auto_clear_interval = 0.f;       /* Timespan of no inputs after which history will be cleared */
        const char* key_name_path = nullptr;    /* Path to additional key name config*/
        const char* icon_path = nullptr;        /* Path to icons used for icon mode */
        const char* icon_cfg_path = nullptr;    /* Path to icon config file */

        /* General values */
        element_data_holder* data = nullptr;    /* Points to selected input source */
        obs_source_t* source = nullptr;         /* input-history source */
        obs_data_t* settings = nullptr;         /* input-history settings (includes text source settings) */
        obs_source_t* text_source = nullptr;    /* Contains text source used for text mode */
        obs_data_t* text_settings = nullptr;    /* Settings for text source */
        uint16_t flags = 0x0;                   /* Contains all settings flags */
        input_queue* queue = nullptr;           /* Contains input entries for visualization*/
        key_names* names = nullptr;             /* Custom key name configuration */
        history_icons* icons = nullptr;         /* Texture and config to display keys as icons */
        uint32_t cx = 25, cy = 25;              /* Source dimensions */
    };

    /* TODO: Rework */
    struct command_handler
    {
        bool m_empty = true;
        std::string commands[MAX_HISTORY_SIZE];

        void finish_command()
        {
            for (auto i = MAX_HISTORY_SIZE - 1; i > 0; i--)
            {
                commands[i] = commands[i - 1];
            }
            commands[0] = "";
        }

        bool special_handling(const wint_t character)
        {
            if (character == CHAR_BACK)
            {
                if (commands[0].length() > 0)
                    commands[0].pop_back();
            }
            else if (character == CHAR_ENTER)
            {
                finish_command();
            }
            return character == CHAR_BACK || character == CHAR_ENTER;
        }

        void clear()
        {
            for (auto& command : commands)
                command = "";
        }

        void handle_char(const wint_t character)
        {
            if (special_handling(character))
                return;

            char buffer[2];
            snprintf(buffer, sizeof(buffer), "%lc", character);
            commands[0].append(buffer);
        }

        std::string get_history(const bool down) const
        {
            std::string result;
            if (down)
            {
                for (auto i = MAX_HISTORY_SIZE - 1; i >= 0; i--)
                {
                    result.append(commands[i]);
                    if (i >= 1)
                        result.append("\n");
                }
            }
            else
            {
                for (auto i = 0; i < MAX_HISTORY_SIZE; i++)
                {
                    result.append(commands[i]);
                    if (i < MAX_HISTORY_SIZE - 1)
                        result.append("\n");
                }
            }
            return result;
        }
    };

    class input_history_source
    {
        command_handler* m_command_handler = nullptr;

        double m_clear_timer = 0.f;
        double m_collect_timer = 0.f;
    public:
        history_settings m_settings;
        
        input_history_source(obs_source_t* source_, obs_data_t* settings);
        ~input_history_source();

        void load_text_source();
        void load_icons();
        void load_translation();
        void load_command_handler();

        inline void unload_text_source();
        inline void unload_icons();
        inline void unload_translation();
        inline void unload_command_handler();

        void clear_history() const;

        inline void update(obs_data_t* settings);
        inline void tick(float seconds);
        inline void render(gs_effect_t* effect) const;

        uint32_t get_width() const { return m_settings.cx; }
        uint32_t get_height() const { return m_settings.cy; }
    };

    /* Util for registering the source */
    static bool clear_history(obs_properties_t* props, obs_property_t* property,
        void* data);

    static bool mode_changed(obs_properties_t* props, obs_property_t* p,
        obs_data_t* s);

    static bool include_pad_changed(obs_properties* props, obs_property_t* p,
        obs_data_t* s);

    static obs_properties_t* get_properties_for_history(void* data);

    void register_history();
};
