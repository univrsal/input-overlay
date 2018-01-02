#include "input-source.hpp"

void InputSource::DrawKey(gs_effect_t *effect, InputKey* key, uint16_t x, uint16_t y)
{
    gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), m_image->texture);

    gs_matrix_push();
    gs_matrix_translate3f((float)x, (float)y, 1.f);
    if (key->m_pressed)
    {
        gs_draw_sprite_subregion(m_image->texture, 0, key->texture_u, key->texture_v + key->h + 3, key->w + 1, key->h + 1);
    }
    else
    {
        gs_draw_sprite_subregion(m_image->texture, 0, key->texture_u, key->texture_v, key->w + 1, key->h + 1);
    }

    gs_matrix_pop();
}

void InputSource::UnloadOverlayTexure()
{
    obs_enter_graphics();
    gs_image_file_free(m_image);
    obs_leave_graphics();
}

std::wstring stringToWString(const std::string& t_str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &t_str[0], (int)t_str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &t_str[0], (int)t_str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

inline void InputSource::Update(obs_data_t *settings)
{
    m_image_file = obs_data_get_string(settings, S_OVERLAY_FILE);
    std::string s = obs_data_get_string(settings, S_LAYOUT_FILE);
    m_layout_file = stringToWString(s);

    LoadOverlayTexture();
    LoadOverlayLayout();

    if (!m_layout.m_is_loaded && m_image && m_image->texture)
    {
        cx = m_image->cx;
        cy = m_image->cy;
    }
}

inline void InputSource::Tick(float seconds)
{
    CheckKeys();
    /*
    int controllerId = -1;

    for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++)
    {
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    if (XInputGetState(i, &state) == ERROR_SUCCESS)
    controllerId = i;
    }

    if (controllerId >= 0) {
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    if (XInputGetState(controllerId, &state) == ERROR_SUCCESS)
    warning("Controller on port %i\n", controllerId);
    bool A_button = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);
    float left_trigger = (float)state.Gamepad.bLeftTrigger / 255;
    warning("Left trigger: %.4f, A button: %s\n", left_trigger, A_button ? "yes" : "no");
    }
    else
    warning("No controller found!\n");*/
}

inline void InputSource::Render(gs_effect_t *effect)
{
    if (!m_image || !m_image->texture)
        return;

    if (m_layout_file.empty() || !m_layout.m_is_loaded) {
        gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), m_image->texture);
        gs_draw_sprite(m_image->texture, 0, cx, cy);
    }
    else {
        int x = 0, y = 0;
        InputKey k;

        if (m_layout.m_type == TYPE_KEYBOARD)
        {
            for (int i = 0; i < m_layout.m_key_count; i++)
            {
                k = m_layout.m_keys[i];
                x = m_layout.m_btn_w * k.column + (m_layout.m_key_space_h * k.column) + k.x_offset;
                y = m_layout.m_btn_h * k.row + (m_layout.m_key_space_v * k.row);
                DrawKey(effect, &k, x, y);
            }
        }
        else if (m_layout.m_type == TYPE_MOUSE)
        {
            for (int i = 0; i < m_layout.m_key_count; i++)
            {
                k = m_layout.m_keys[i];
                x = k.column;
                y = k.row;
                DrawKey(effect, &k, x, y);
            }
        }

    }
}

void InputSource::LoadOverlayTexture()
{
    UnloadOverlayTexure();

    if (!m_image_file.empty())
    {
        if (m_image == nullptr)
        {
            m_image = new gs_image_file_t();
            warning("Error: Image struct was null!");
        }

        warning("Loading texture %s as overlay base image!", m_image_file.c_str());
        gs_image_file_init(m_image, m_image_file.c_str());

        obs_enter_graphics();
        gs_image_file_init_texture(m_image);
        obs_leave_graphics();

        if (!m_image->loaded)
        {
            warning("Error: failed to load texture %s", m_image_file.c_str());
        }
    }
}

void InputSource::LoadOverlayLayout()
{
    m_layout.m_is_loaded = false;

    if (m_layout_file.empty())
        return;

    UnloadOverlayLayout();
    std::ifstream layout_file(m_layout_file.c_str());

    if (!layout_file.is_open())
        return;

    std::string line;
    std::string key_order, key_width, key_height, key_col, key_row;

    obs_data *settings = obs_source_get_settings(m_source);
    std::string path = obs_data_get_string(settings, S_LAYOUT_FILE);
    ccl_config *cfg = new ccl_config(path.c_str(), "");

    if (!cfg->is_empty())
    {
        m_layout.m_type = (LayoutType)cfg->get_int("layout_type");
        m_layout.m_key_count = cfg->get_int("key_count");

        if (m_layout.m_type == TYPE_KEYBOARD) {
            m_layout.m_rows = cfg->get_int("key_rows");
            m_layout.m_cols = cfg->get_int("key_cols");
            m_layout.m_btn_w = cfg->get_int("key_abs_w");
            m_layout.m_btn_h = cfg->get_int("key_abs_h");
            m_layout.m_key_space_v = cfg->get_int("key_space_v");
            m_layout.m_key_space_h = cfg->get_int("key_space_h");
            key_order = cfg->get_string("key_order");
            key_width = cfg->get_string("key_width");
            key_height = cfg->get_string("key_height");
            key_col = cfg->get_string("key_col");
            key_row = cfg->get_string("key_row");
            m_layout.texture_w = cfg->get_int("texture_w");
            m_layout.texture_v_space = cfg->get_int("texture_v_space");

            uint16_t u_cord = 1, v_cord = 1;
            uint16_t tempw, temph;
            int index = 0;

            for (int i = 0; i < m_layout.m_key_count; i++)
            {
                if (index >= m_layout.texture_w)
                {
                    index = 0;
                    u_cord = 1;
                    v_cord += m_layout.texture_v_space + 6;
                }

                InputKey k;
                k.texture_u = u_cord - 1;
                k.texture_v = v_cord - 1;
                tempw = read_chain_int(key_width);
                temph = read_chain_int(key_height);
                k.w = tempw * m_layout.m_btn_w;
                k.h = temph * m_layout.m_btn_h;
                k.m_key_char = read_chain(key_order);
                k.m_pressed = false;
                k.row = read_chain_int(key_row);
                k.column = read_chain_int(key_col);

                if (tempw > 1)
                {
                    k.x_offset = (m_layout.m_btn_w * tempw + m_layout.m_key_space_h * (tempw - 1)) / 2 - k.w / 2;
                    index += tempw;
                }
                else
                {
                    index++;
                    k.x_offset = 0;
                }

                m_layout.m_keys.emplace_back(k);
                u_cord += k.w + 3;

            }

            m_layout.m_h = m_layout.m_rows * m_layout.m_btn_h + m_layout.m_key_space_v * m_layout.m_rows;
            m_layout.m_w = m_layout.m_cols * m_layout.m_btn_w + m_layout.m_key_space_h * (m_layout.m_cols - 1);
        }
        else if (m_layout.m_type = TYPE_MOUSE)
        {

            line = cfg->get_string("mouse_layout_w_h");
            m_layout.m_w = read_chain_int(line);
            m_layout.m_h = read_chain_int(line);

            // Stitching together config value identifiers (what a mess)

            std::string vals[] = { "lmb", "rmb", "mmb", "smb1", "smb2", "body" };
            unsigned char keys[]{ KEY_LMB, KEY_RMB, KEY_MMB, KEY_SMB1, KEY_SMB2, 0 };

            std::stringstream stream;
            std::string begin;

            for (int i = 0; i < 6; i++)
            {
                InputKey m;
                m.m_key_char = keys[i];

                stream.str("");
                begin = "mouse_" + vals[i];
                stream << begin << "_u_v";

                line = cfg->get_string(stream.str().c_str());
                m.texture_u = read_chain_int(line);
                m.texture_v = read_chain_int(line);

                stream.str("");
                stream << begin << "_w_h";

                line = cfg->get_string(stream.str().c_str());
                m.w = read_chain_int(line);
                m.h = read_chain_int(line);

                stream.str("");
                stream << begin << "_x_y";

                line = cfg->get_string(stream.str().c_str());
                m.column = read_chain_int(line);
                m.row = read_chain_int(line);

                m_layout.m_keys.emplace_back(m);
            }
        }
        else if (m_layout.m_type == TYPE_CONTROLLER)
        {

        }

        m_layout.m_key_count = min(m_layout.m_key_count, m_layout.m_keys.size());
        m_layout.m_is_loaded = true;

        cx = m_layout.m_w;
        cy = m_layout.m_h;
    }
}

void InputSource::UnloadOverlayLayout()
{
    if (!m_layout.m_keys.empty())
        m_layout.m_keys.clear();
}

void InputSource::CheckKeys()
{
    if (m_layout.m_is_loaded) {
        for (int i = 0; i < m_layout.m_key_count; i++)
        {
            m_layout.m_keys[i].m_pressed = GetAsyncKeyState(m_layout.m_keys[i].m_key_char) & SHIFTED;
        }
    }
}

obs_properties_t * get_properties_for_overlay(void * data)
{
    InputSource *s = reinterpret_cast<InputSource*>(data);

    obs_properties_t *props = obs_properties_create();

    std::string img_path;
    std::string layout_path;
    std::string filter_img;
    std::string filter_text;

    filter_text += T_FILTER_TEXT_FILES;
    filter_text += " (*.ini);;";
    filter_text += T_FILTER_ALL_FILES;
    filter_text += " (*.*)";

    filter_img += T_FILTER_IMAGE_FILES;
    filter_img += " (*.jpg *.png *.bmp);;";
    filter_img += T_FILTER_ALL_FILES;
    filter_img += " (*.*)";

    if (s && !s->m_image_file.empty()) {
        const char *slash;

        img_path = s->m_image_file;
        replace(img_path.begin(), img_path.end(), '\\', '/');
        slash = strrchr(img_path.c_str(), '/');
        if (slash)
            img_path.resize(slash - img_path.c_str() + 1);
    }

    if (s && !s->m_layout_file.empty()) {
        const char *slash;

        // Wtf.
        std::wstring ws = s->m_layout_file;
        setlocale(LC_ALL, "");
        const std::locale locale("");
        typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
        const converter_type& converter = std::use_facet<converter_type>(locale);
        std::vector<char> to(ws.length() * converter.max_length());
        std::mbstate_t state;
        const wchar_t* from_next;
        char* to_next;
        const converter_type::result result = converter.out(state, ws.data(), ws.data() + ws.length(), from_next, &to[0], &to[0] + to.size(), to_next);

        if (result == converter_type::ok || result == converter_type::noconv)
        {
            layout_path = std::string(&to[0], to_next);
            replace(layout_path.begin(), layout_path.end(), '\\', '/');
            slash = strrchr(layout_path.c_str(), '/');
            if (slash)
                layout_path.resize(slash - layout_path.c_str() + 1);
        }


    }

    obs_properties_add_path(props, S_OVERLAY_FILE, T_OVERLAY_FILE, OBS_PATH_FILE,
        filter_img.c_str(), img_path.c_str());

    obs_properties_add_path(props, S_LAYOUT_FILE, T_LAYOUT_FILE, OBS_PATH_FILE,
        filter_text.c_str(), layout_path.c_str());
    return props;
}

void register_overlay_source()
{
    // Input Overlay
    obs_source_info si = {};
    si.id = "input-overlay";
    si.type = OBS_SOURCE_TYPE_INPUT;
    si.output_flags = OBS_SOURCE_VIDEO;
    si.get_properties = get_properties_for_overlay;

    si.get_name = [](void*) { return obs_module_text("InputOverlay"); };
    si.create = [](obs_data_t *settings, obs_source_t *source) { return (void*) new InputSource(source, settings);    };
    si.destroy = [](void *data) { delete reinterpret_cast<InputSource*>(data);    };
    si.get_width = [](void *data) { return reinterpret_cast<InputSource*>(data)->cx;    };
    si.get_height = [](void *data) { return reinterpret_cast<InputSource*>(data)->cy;    };

    si.get_defaults = [](obs_data_t *settings)
    {
        obs_data_t *input_obj = obs_data_create();
        obs_data_release(input_obj);
    };

    si.update = [](void *data, obs_data_t *settings) { reinterpret_cast<InputSource*>(data)->Update(settings); };
    si.video_tick = [](void *data, float seconds) { reinterpret_cast<InputSource*>(data)->Tick(seconds); };
    si.video_render = [](void *data, gs_effect_t *effect) { reinterpret_cast<InputSource*>(data)->Render(effect); };
    obs_register_source(&si);
}