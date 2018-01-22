#include "input-history.hpp"


void InputHistorySource::unload_textsource(void)
{
    if (m_text_source)
    {
        obs_source_remove(m_text_source);
        obs_source_release(m_text_source);
        m_text_source = NULL;
    }
}

bool InputHistorySource::is_pressed(int k)
{
    return util_key_exists(k);
}

void InputHistorySource::add_to_history(KeyBundle b)
{
    m_history[4] = m_history[3];
    m_history[3] = m_history[2];
    m_history[2] = m_history[1];
    m_history[1] = m_history[0];
    m_history[0] = b;
}

void InputHistorySource::clear_history(void)
{

    for (int i = 0; i < MAX_HISTORY_SIZE; i++)
    {
        m_history[i] = {};
    }
    
    m_prev_keys = {};
    m_current_keys = {};

    obs_data_t* text_data = obs_source_get_settings(m_text_source);
    obs_data_t* main_data = obs_source_get_settings(m_source);
    
    obs_data_set_string(text_data, "text", "");
    obs_source_update(m_text_source, text_data);
    obs_source_update(m_source, main_data);
}

KeyBundle InputHistorySource::check_keys(void)
{
    KeyBundle temp = KeyBundle();
    if (!util_pressed_empty())
    {
        
        for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
        {
            temp.m_keys[i] = pressed_keys[i];
            if (pressed_keys[i] > 0)
                temp.m_empty = false;
        }
    }

    return temp;
}

inline void InputHistorySource::Update(obs_data_t * settings)
{
    obs_source_update(m_text_source, settings);

    m_history_size = obs_data_get_int(settings, S_OVERLAY_HISTORY_SIZE);   
    m_fix_cutting = obs_data_get_bool(settings, S_OVERLAY_FIX_CUTTING);
    m_include_mouse = obs_data_get_bool(settings, S_OVERLAY_INCLUDE_MOUSE);
    m_dir_up = obs_data_get_bool(settings, S_OVERLAY_DIRECTION);
    m_update_interval = obs_data_get_int(settings, S_OVERLAY_INTERVAL);
    m_repeat_keys = obs_data_get_bool(settings, S_OVERLAY_ENABLE_REPEAT_KEYS);
    m_auto_clear = obs_data_get_bool(settings, S_OVERLAY_ENABLE_AUTO_CLEAR);
    m_clear_interval = obs_data_get_int(settings, S_OVERLAY_AUTO_CLEAR_INTERVAL);
}

#define START (m_dir_up ? m_history_size - 1 : 0)
#define CONDITION (m_dir_up ? (i >= 0) : (i < m_history_size))
#define INCREMENT (i += (m_dir_up ? -1 : 1))

inline void InputHistorySource::Tick(float seconds)
{
    if (!m_source || !obs_source_showing(m_source))
        return;

    if (m_auto_clear)
    {
        m_clear_timer += seconds;
        if (m_clear_timer >= m_clear_interval)
        {
            m_clear_timer = 0.f;
            clear_history();
        }
    }

    if (m_counter >= m_update_interval)
    {
        m_counter = 0;
        if (!m_current_keys.m_empty)
        {
          if (m_repeat_keys || !m_current_keys.compare(&m_prev_keys))
            {
                if (!m_current_keys.is_only_mouse() || m_include_mouse)
                {
                    add_to_history(m_current_keys);
                    m_clear_timer = 0.f;
                }

                m_prev_keys = m_current_keys;
                
                std::string text = "";
                std::string line = "";

                for (int i = START; CONDITION; INCREMENT)
                {
                    if (m_history[i].m_empty)
                    {
                        text.append("\n");
                        continue;
                    }

                    line = m_history[i].to_string(m_fix_cutting, m_include_mouse);

                    if (!line.empty())
                    {
                        if (!m_dir_up && i > 0 || m_dir_up && i < m_history_size - 1)
                            text.append("\n");
                        text.append(line);
                    }
                }
                
                obs_data_t* text_data = obs_source_get_settings(m_text_source);
                obs_data_t* main_data = obs_source_get_settings(m_source);

                obs_data_set_string(text_data, "text", text.c_str());
                obs_source_update(m_text_source, text_data);
                obs_source_update(m_source, main_data);

                m_current_keys = {};
            }
        }
    }
    else
    {
        m_current_keys.merge(check_keys());
        m_counter++;
    }
    
    cx = UTIL_MAX(obs_source_get_width(m_text_source), 50);
    cy = UTIL_MAX(obs_source_get_height(m_text_source), 50);
}

inline void InputHistorySource::Render(gs_effect_t * effect)
{
    obs_source_video_render(m_text_source);
}

void KeyBundle::merge(KeyBundle other)
{
    if (!other.m_empty)
    {
        m_empty = false;

        for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
        {
            if (other.m_keys[i] > 0)
                m_keys[i] = other.m_keys[i];
        }
    }
}

std::string KeyBundle::to_string(bool fix, bool include_mouse)
{
    if (m_empty)
        return "";

    std::string text;
    bool flag = false;

    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
    {
        if (m_keys[i] == 0)
            break; // Array is filled from beginning to end
                   // -> First entry with zero means there are none after it
        
        if (!include_mouse)
        {
            switch (m_keys[i])
            {
                case VC_MOUSE_BUTTON1:
                case VC_MOUSE_BUTTON2:
                case VC_MOUSE_BUTTON3:
                case VC_MOUSE_BUTTON4:
                case VC_MOUSE_BUTTON5:
                case VC_MOUSE_WHEEL_UP:
                case VC_MOUSE_WHEEL_DOWN:
                    continue;
            }
        }

        if (flag)
            text.append(" + ");
        else
            flag = true;

        if (m_keys[i] > 0)
        {
            const char* temp = key_to_text(m_keys[i]);
            if (temp)
            {
                text.append(temp);
            }
            else
            {
                std::stringstream stream;
                stream << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2)
                       << std::hex << m_keys[i];
                text.append(stream.str());
            }
        }
           
    }

    if (fix && !text.empty())
        text.append(" ");
    return text;
}

bool KeyBundle::compare(KeyBundle * other)
{
    if (m_empty != other->m_empty)
        return false;

    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
    {
        if (m_keys[i] != other->m_keys[i])
            return false;
    }
    return true;
}

bool KeyBundle::is_only_mouse()
{
    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
    {
        switch (m_keys[i])
        {
        case 0:
        case VC_MOUSE_BUTTON1:
        case VC_MOUSE_BUTTON2:
        case VC_MOUSE_BUTTON3:
        case VC_MOUSE_BUTTON4:
        case VC_MOUSE_BUTTON5:
        case VC_MOUSE_WHEEL_UP:
        case VC_MOUSE_WHEEL_DOWN:
            break;
        default:
            return false;
        }
    }
    return true;
}

bool clear_history(obs_properties_t * props, obs_property_t * property, void * data)
{
    InputHistorySource *source = reinterpret_cast<InputHistorySource*>(data);
    source->clear_history();
    return true;
}

obs_properties_t * get_properties_for_history(void * data)
{
    InputHistorySource *s = reinterpret_cast<InputHistorySource*>(data);

    obs_properties_t *props = obs_properties_create();

    // font settings
    obs_properties_add_font(props, S_OVERLAY_FONT, T_OVERLAY_FONT);
    obs_properties_add_color(props, S_OVERLAY_FONT_COLOR, T_OVERLAY_FONT_COLOR);
    obs_properties_add_int_slider(props, S_OVERLAY_OPACITY, T_OVERLAY_OPACITY, 0, 100, 1);
    obs_properties_add_bool(props, S_OVERLAY_OUTLINE, T_OVERLAY_OUTLINE);

    obs_properties_add_int(props, S_OVERLAY_OUTLINE_SIZE, T_OVERLAY_OUTLINE_SIZE, 1, 20, 1);
    obs_properties_add_color(props, S_OVERLAY_OUTLINE_COLOR, T_OVERLAY_OUTLINE_COLOR);
    obs_properties_add_int_slider(props, S_OVERLAY_OUTLINE_OPACITY,
        T_OVERLAY_OUTLINE_OPACITY, 0, 100, 1);

    // Other
    obs_properties_add_bool(props, S_OVERLAY_DIRECTION, T_OVERLAY_DIRECTION_LABEL);
    obs_properties_add_int(props, S_OVERLAY_HISTORY_SIZE, T_OVERLAY_HISTORY_SIZE, 1, MAX_HISTORY_SIZE, 1);
    obs_properties_add_bool(props, S_OVERLAY_FIX_CUTTING, T_OVERLAY_FIX_CUTTING);
    obs_properties_add_bool(props, S_OVERLAY_INCLUDE_MOUSE, T_OVERLAY_INCLUDE_MOUSE);
    obs_properties_add_int(props, S_OVERLAY_INTERVAL, T_OVERLAY_INTERVAL, 1, 1000, 1);

    obs_properties_add_bool(props, S_OVERLAY_ENABLE_REPEAT_KEYS, T_OVERLAY_ENABLE_REPEAT_KEYS);
    obs_properties_add_bool(props, S_OVERLAY_ENABLE_AUTO_CLEAR, T_OVERLAY_ENABLE_AUTO_CLEAR);
    obs_properties_add_int(props, S_OVERLAY_AUTO_CLEAR_INTERVAL, T_OVERLAY_AUTO_CLEAR_INTERVAL, 1, 30, 1);

    obs_properties_add_button(props, S_OVERLAY_CLEAR_HISTORY, T_OVERLAY_CLEAR_HISTORY, clear_history);
    return props;
}

void register_history()
{
    // Input History
    obs_source_info si = {};
    si.id = "input-history";
    si.type = OBS_SOURCE_TYPE_INPUT;
    si.output_flags = OBS_SOURCE_VIDEO;
    si.get_properties = get_properties_for_history;

    si.get_name = [](void*) { return obs_module_text("InputHistory"); };
    si.create = [](obs_data_t *settings, obs_source_t *source) { return (void*) new InputHistorySource(source, settings); };
    si.destroy = [](void *data) { delete reinterpret_cast<InputHistorySource*>(data); };
    si.get_width = [](void *data) { return reinterpret_cast<InputHistorySource*>(data)->cx; };
    si.get_height = [](void *data) { return reinterpret_cast<InputHistorySource*>(data)->cy; };

    si.get_defaults = [](obs_data_t *settings)
    {
        obs_data_set_default_int(settings, S_OVERLAY_HISTORY_SIZE, 1);
        obs_data_set_default_int(settings, S_OVERLAY_INTERVAL, 2);
        obs_data_set_default_int(settings, S_OVERLAY_AUTO_CLEAR_INTERVAL, 2);
    };

    si.update = [](void *data, obs_data_t *settings) { reinterpret_cast<InputHistorySource*>(data)->Update(settings); };
    si.video_tick = [](void *data, float seconds) { reinterpret_cast<InputHistorySource*>(data)->Tick(seconds); };
    si.video_render = [](void *data, gs_effect_t *effect) { reinterpret_cast<InputHistorySource*>(data)->Render(effect); };
    obs_register_source(&si);
}
