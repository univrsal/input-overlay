#include "inputhistory.hpp"

static int SPECIAL_KEYS[] = { VK_RETURN, VK_BACK, VK_TAB, VK_CAPITAL, VK_ESCAPE, VK_SCROLL, VK_PAUSE,
VK_NUMLOCK };

void InputHistorySource::UnloadTextSource(void)
{
    if (m_text_source)
    {
        obs_source_remove(m_text_source);
        obs_source_release(m_text_source);
    }
    m_text_source = nullptr;
}

bool InputHistorySource::any_key_down(void)
{
    bool mods = is_pressed(VK_CONTROL) || is_pressed(VK_MENU) || is_pressed(VK_SHIFT) || is_pressed(VK_LWIN)
        || is_pressed(VK_RWIN) || (m_include_mouse && (is_pressed(VK_LBUTTON) || is_pressed(VK_RBUTTON) || is_pressed(VK_MBUTTON)));

    bool special = false;
    for (int i = 0; i < SPECIAL_SIZE; i++)
    {
        if (is_pressed(SPECIAL_KEYS[i]))
        {
            special = true;
            break;
        }
    }

    return special || mods || check_range(ALPHABET_START, ALPHABET_END) || check_range(NUMBER_START, NUMBER_END)
        || check_range(NUMPAD_START, NUMPAD_END) || check_range(FUNCTION_START, FUNCTION_END)
        || check_range(UTIL_START, UTIL_END);
}

bool InputHistorySource::check_range(int start, int end)
{
    for (int i = start; i <= end; i++)
        if (GetAsyncKeyState(i) & SHIFTED)
            return true;
    return false;
}

bool InputHistorySource::is_pressed(int k)
{
    return GetAsyncKeyState(k) & SHIFTED;
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
    for (int i = 0; i < 4; i++)
    {
        m_history[i] = {};
    }
    obs_data_t* data = obs_source_get_settings(m_text_source);
    obs_data_set_string(data, "text", "");
    obs_source_update(m_text_source, data);
}

KeyBundle InputHistorySource::check_keys(void)
{
    KeyBundle temp = KeyBundle();
    temp.m_empty = false;
    temp.m_mod_ctrl = is_pressed(VK_CONTROL);
    temp.m_mod_shift = is_pressed(VK_SHIFT);
    temp.m_mod_alt = is_pressed(VK_MENU);
    temp.m_mod_win = is_pressed(VK_LWIN) || is_pressed(VK_RWIN);
    temp.m_lmb = is_pressed(VK_LBUTTON);
    temp.m_rmb = is_pressed(VK_RBUTTON);
    temp.m_mmb = is_pressed(VK_MBUTTON);
    temp.m_pmb = is_pressed(VK_XBUTTON1);
    temp.m_nmb = is_pressed(VK_XBUTTON2);

    int keys = 0;

    for (int i = ALPHABET_START; i <= ALPHABET_END; i++)
    {
        if (is_pressed(i))
        {
            temp.m_keys[keys] = i;
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }

    for (int i = NUMBER_START; i <= NUMBER_END; i++)
    {
        if (is_pressed(i))
        {
            temp.m_keys[keys] = i;
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }

    for (int i = NUMPAD_START; i <= NUMPAD_END; i++)
    {
        if (is_pressed(i))
        {
            temp.m_keys[keys] = i;
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }

    for (int i = FUNCTION_START; i <= FUNCTION_END; i++)
    {
        if (is_pressed(i))
        {
            temp.m_keys[keys] = i;
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }

    for (int i = 0; i < SPECIAL_SIZE; i++)
    {
        if (is_pressed(SPECIAL_KEYS[i]))
        {
            temp.m_keys[keys] = SPECIAL_KEYS[i];
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }

    for (int i = UTIL_START; i <= UTIL_END; i++)
    {
        if (is_pressed(i))
        {
            temp.m_keys[keys] = i;
            keys++;
        }

        if (keys == 4)
        {
            return temp;
        }
    }
    return temp;
}

inline void InputHistorySource::Update(obs_data_t * settings)
{
    m_history_size = obs_data_get_int(settings, S_OVERLAY_HISTORY_SIZE);
    obs_source_update(m_text_source, settings);

    m_fix_cutting = obs_data_get_bool(settings, S_OVERLAY_FIX_CUTTING);
    m_include_mouse = obs_data_get_bool(settings, S_OVERLAY_INCLUDE_MOUSE);
    m_dir_up = obs_data_get_bool(settings, S_OVERLAY_DIRECTION);
    m_update_interval = obs_data_get_int(settings, S_OVERLAY_INTERVAL);
    m_repeat_keys = obs_data_get_bool(settings, S_OVERLAY_ENABLE_REPEAT_KEYS);
    m_auto_clear = obs_data_get_bool(settings, S_OVERLAY_ENABLE_AUTO_CLEAR);
    m_clear_interval = obs_data_get_int(settings, S_OVERLAY_AUTO_CLEAR_INTERVAL);
}

inline void InputHistorySource::Tick(float seconds)
{
    if (!obs_source_showing(m_source))
        return;

    cx = max(obs_source_get_width(m_text_source), 50);
    cy = max(obs_source_get_height(m_text_source), 50);

    if (m_auto_clear)
    {
        m_clear_timer += seconds;
        if (m_clear_timer >= m_clear_interval)
        {
            m_clear_timer = 0.f;
            clear_history();
        }
    }

    if (m_update_keys)
    {
        obs_data_t* data = obs_source_get_settings(m_text_source);
        m_update_keys = false;
        if (!m_current_keys.m_empty && (m_repeat_keys
            || !m_current_keys.compare(&m_prev_keys))) {

            add_to_history(m_current_keys);
            m_clear_timer = 0.f;
        }

        m_prev_keys = m_current_keys;
        char* text = "";
        char* line_text = "";
        int i = m_dir_up ? m_history_size - 1 : 0;
        for (;;)
        {
            if (!m_dir_up && i >= m_history_size || m_dir_up && i < 0)
                break;
            if (m_history[i].m_empty)
            {
                text = append(text, "\n");
                i += m_dir_up ? -1 : 1;
                continue;
            }

            line_text = (char*)m_history[i].to_string(m_fix_cutting, m_include_mouse);

            if (strlen(line_text) > 0)
            {
                if (!m_dir_up && i > 0 || m_dir_up && i < m_history_size - 1)
                    text = append(text, "\n");
                text = append(text, line_text);
            }

            i += m_dir_up ? -1 : 1;
        }

        obs_data_set_string(data, "text", text);
        obs_source_update(m_text_source, data);
        m_current_keys = {};
    }
    else
    {
        if (any_key_down())
        {
            m_collect_keys = true;
        }

        if (m_collect_keys)
        {
            KeyBundle temp = check_keys();
            m_current_keys = m_current_keys.merge(temp);
            m_counter++;
            if (m_counter >= m_update_interval)
            {
                m_collect_keys = false;
                m_update_keys = true;
                m_counter = 0;
            }
        }
    }
}

inline void InputHistorySource::Render(gs_effect_t * effect)
{
    obs_source_video_render(m_text_source);
}

KeyBundle KeyBundle::merge(KeyBundle other)
{
    KeyBundle c;
    if (other.m_empty)
        return c;
    c.m_empty = false;
    c.m_mmb = other.m_mmb || m_mmb;
    c.m_lmb = other.m_lmb || m_lmb;
    c.m_rmb = other.m_rmb || m_rmb;
    c.m_mod_alt = other.m_mod_alt || m_mod_alt;
    c.m_mod_ctrl = other.m_mod_ctrl || m_mod_ctrl;
    c.m_mod_shift = other.m_mod_shift || m_mod_shift;
    c.m_mod_win = other.m_mod_win || m_mod_win;

    for (int i = 0; i < 4; i++)
    {
        if (other.m_keys[i] > 0)
            c.m_keys[i] = other.m_keys[i];
        else
            c.m_keys[i] = m_keys[i];
    }
    return c;
}

const char * KeyBundle::to_string(bool fix, bool include_mouse)
{
    if (m_empty)
        return "";

    char* text = "";
    bool flag = false;

    if (m_mod_shift)
        text = add_key(text, "SHIFT", &flag);

    if (m_mod_ctrl)
        text = add_key(text, "CTRL", &flag);

    if (m_mod_alt)
        text = add_key(text, "ALT", &flag);

    if (m_mod_win)
        text = add_key(text, "WIN", &flag);

    if (include_mouse)
    {
        if (m_lmb)
            text = add_key(text, "LEFT CLICK", &flag);
        if (m_rmb)
            text = add_key(text, "RIGHT CLICK", &flag);
        if (m_mmb)
            text = add_key(text, "MIDDLE CLICK", &flag);
    }

    for (int i = 0; i < 4; i++)
    {
        if (m_keys[i] > 0)
            text = add_key(text, (char*)key_to_text(m_keys[i]), &flag);
    }

    if (fix)
        text = append(text, " ");
    return text;
}

char * KeyBundle::add_key(char * text, char * key, bool * flag)
{
    if (*flag)
        text = append(text, " + ");
    *flag = true;
    text = append(text, key);
    return text;
}

bool KeyBundle::compare(KeyBundle * other)
{
    if (m_empty != other->m_empty)
        return false;

    if (m_lmb != other->m_lmb || m_rmb != other->m_rmb || m_mmb != other->m_mmb
        || m_pmb != other->m_pmb || m_nmb != other->m_nmb)
        return false;

    if (m_mod_alt != other->m_mod_alt || m_mod_ctrl != other->m_mod_ctrl
        || m_mod_win != other->m_mod_win || m_mod_shift != other->m_mod_shift)
        return false;

    for (int i = 0; i < 4; i++)
    {
        if (m_keys[i] != other->m_keys[i])
            return false;
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

    obs_properties_add_bool(props, S_OVERLAY_DIRECTION, T_OVERLAY_DIRECTION_LABEL);
    obs_properties_add_int(props, S_OVERLAY_HISTORY_SIZE, T_OVERLAY_HISTORY_SIZE, 1, 5, 1);
    obs_properties_add_bool(props, S_OVERLAY_FIX_CUTTING, T_OVERLAY_FIX_CUTTING);
    obs_properties_add_bool(props, S_OVERLAY_INCLUDE_MOUSE, T_OVERLAY_INCLUDE_MOUSE);
    obs_properties_add_int(props, S_OVERLAY_INTERVAL, T_OVERLAY_INTERVAL, 1, 1000, 1);

    obs_properties_add_bool(props, S_OVERLAY_ENABLE_REPEAT_KEYS, T_OVERLAY_ENABLE_REPEAT_KEYS);
    obs_properties_add_bool(props, S_OVERLAY_ENABLE_AUTO_CLEAR, T_OVERLAY_ENABLE_AUTO_CLEAR);
    obs_properties_add_int(props, S_OVERLAY_AUTO_CLEAR_INTERVAL, T_OVERLAY_AUTO_CLEAR_INTERVAL, 1, 120, 1);

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
        obs_data_t *input_obj = obs_data_create();
        obs_data_release(input_obj);
    };

    si.update = [](void *data, obs_data_t *settings) { reinterpret_cast<InputHistorySource*>(data)->Update(settings); };
    si.video_tick = [](void *data, float seconds) { reinterpret_cast<InputHistorySource*>(data)->Tick(seconds); };
    si.video_render = [](void *data, gs_effect_t *effect) { reinterpret_cast<InputHistorySource*>(data)->Render(effect); };
    obs_register_source(&si);
}