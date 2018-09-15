/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <obs-module.h>
#include <map>
#include "input_source.hpp"
#include "../../ccl/ccl.hpp"
#include "../util/util.hpp"
#include "../util/layout_constants.hpp"
#include "../hook/gamepad_hook.hpp"
#include "../hook/hook_helper.hpp"

extern "C" {
#include <graphics/image-file.h>
}


#define MAX_HISTORY_SIZE 5
#define MAX_SIMULTANEOUS_KEYS 10

#define SET_MASK(a, b)      (util_set_mask(m_bool_values, a, b))
#define GET_MASK(a)         (m_bool_values & a)

#define MASK_TEXT_MODE      1 << 0
#define MASK_FIX_CUTTING    1 << 1
#define MASK_INCLUDE_MOUSE  1 << 2
#define MASK_UPDATE_KEYS    1 << 3
#define MASK_AUTO_CLEAR     1 << 4
#define MASK_REPEAT_KEYS    1 << 5
#define MASK_TRANSLATION    1 << 6
#define MASK_USE_FALLBACK   1 << 7
#define MASK_COMMAND_MODE   1 << 8
#define MASK_INCLUDE_PAD    1 << 9

namespace sources
{
    class key_names;
    class key_bundle;

    class key_names
    {
    public:
        void load_from_file(const std::string& path);
        const char* get_name(uint16_t vc);

        ~key_names();

    private:
        std::map<uint16_t, std::string> m_names;
    };

    class key_bundle
    {
    public:
        bool m_empty = true;
        uint16_t m_keys[MAX_SIMULTANEOUS_KEYS] = {0};

        void merge(key_bundle other);

        std::string to_string(uint8_t masks, key_names* names);
        bool compare(key_bundle* other);
        bool is_only_mouse();
        void add_key(uint16_t key);
    private:
        uint8_t m_index = 0;
    };

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

    struct key_icon
    {
        uint16_t u, v;
    };

    struct key_icons
    {
        ~key_icons();

        void load_from_file(const std::string& img_path,
                            const std::string& cfg_path);
        key_icon* get_icon_for_key(uint16_t vc);

        uint16_t get_w() const { return m_icon_w; }
        uint16_t get_h() const { return m_icon_h; }

        bool is_loaded() const { return m_loaded; }
        bool has_texture_for_bundle(key_bundle* bundle);

        gs_image_file_t* get_texture() const { return m_icon_texture; }

    private:
        bool m_loaded = false;
        uint16_t m_icon_count = 0;
        uint16_t m_icon_w = 0;
        uint16_t m_icon_h = 0;
        std::map<uint16_t, key_icon> m_icons;
        void unload_texture();
        gs_image_file_t* m_icon_texture = nullptr;
    };

    struct input_history_source
    {
        obs_source_t* m_source = nullptr;
        obs_data_t* m_settings = nullptr;
        obs_source_t* m_text_source = nullptr;

        uint8_t m_history_size = 1;
        uint8_t m_pad_id = 0;

        uint32_t cx = 0;
        uint32_t cy = 0;
        uint32_t m_update_interval = 1, m_counter = 0;
        int16_t m_icon_v_space = 0, m_icon_h_space = 0;

        uint16_t m_bool_values = 0x0000;
        icon_direction m_history_direction = DIR_DOWN;

        key_bundle m_current_keys;
        key_bundle m_prev_keys;
        key_bundle m_history[MAX_HISTORY_SIZE];

        std::string m_key_name_path;
        std::string m_key_icon_path;
        std::string m_key_icon_config_path;

        key_names* m_key_names = nullptr;
        key_icons* m_key_icons = nullptr;
        command_handler* m_command_handler = nullptr;

        float m_clear_timer = 0.f;
        int m_clear_interval = 0;

        input_history_source(obs_source_t* source_, obs_data_t* settings) :
            m_source(source_),
            m_settings(settings)
        {
            obs_source_update(m_source, settings);
            load_text_source();
        }

        ~input_history_source()
        {
            unload_text_source();
            unload_icons();
            unload_translation();
            unload_command_handler();
        }

        void load_text_source();
        void load_icons();
        void load_translation();
        void load_command_handler();

        inline void unload_text_source();
        inline void unload_icons();
        inline void unload_translation();
        inline void unload_command_handler();

        key_bundle check_keys() const;
        /* Checks currently pressed keys and puts them in a bundle */
        void add_to_history(key_bundle b);
        void clear_history();
        void handle_text_history();
        void handle_icon_history(gs_effect_t* effect);

        inline void update(obs_data_t* settings);
        inline void tick(float seconds);
        inline void render(gs_effect_t* effect);
    };

    // Util for registering the source
    static bool clear_history(obs_properties_t* props, obs_property_t* property,
                              void* data);

    static bool mode_changed(obs_properties_t* props, obs_property_t* p,
                             obs_data_t* s);

    static bool include_pad_changed(obs_properties* props, obs_property_t* p,
                                    obs_data_t* s);

    static obs_properties_t* get_properties_for_history(void* data);

    void register_history();
};
