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
void InputSource::DrawKey(gs_effect_t * effect, InputKey * key)
{
    DrawKey(effect, key, key->column, key->row);
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
    m_pad_settings.m_controller_id = obs_data_get_int(settings, S_CONTROLLER_ID);
    m_pad_settings.m_left_dead_zone = obs_data_get_int(settings, S_CONTROLLER_L_DEAD_ZONE);
    m_pad_settings.m_right_dead_zone = obs_data_get_int(settings, S_CONTROLLER_R_DEAD_ZONE);
    
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
        else if (m_layout.m_type == TYPE_CONTROLLER)
        {
            DrawKey(effect, &m_layout.m_keys[PAD_LT]);
            DrawKey(effect, &m_layout.m_keys[PAD_RT]);
            DrawKey(effect, &m_layout.m_keys[PAD_RB]);
            DrawKey(effect, &m_layout.m_keys[PAD_LB]);

            DrawKey(effect, &m_layout.m_keys[PAD_BODY], 0, 0);
            
            InputKey k = m_layout.m_keys[PAD_L_ANALOG];
            x = k.column - k.w / 2 + m_pad_settings.m_analog_radius * m_pad_settings.m_left_analog_x;
            y = k.row - k.h / 2 - m_pad_settings.m_analog_radius * m_pad_settings.m_left_analog_y;
            DrawKey(effect, &k, x, y);

            k = m_layout.m_keys[PAD_R_ANALOG];
            x = k.column - k.w / 2 + m_pad_settings.m_analog_radius * m_pad_settings.m_right_analog_x;
            y = k.row - k.h / 2 - m_pad_settings.m_analog_radius * m_pad_settings.m_right_analog_y;
            DrawKey(effect, &k, x, y);

            DrawKey(effect, &m_layout.m_keys[PAD_BACK]);
            DrawKey(effect, &m_layout.m_keys[PAD_START]);
            DrawKey(effect, &m_layout.m_keys[PAD_PLAYER_1 + m_pad_settings.m_controller_id]);
            DrawKey(effect, &m_layout.m_keys[PAD_X]);
            DrawKey(effect, &m_layout.m_keys[PAD_Y]);
            DrawKey(effect, &m_layout.m_keys[PAD_A]);
            DrawKey(effect, &m_layout.m_keys[PAD_B]);
            DrawKey(effect, &m_layout.m_keys[PAD_DPAD_UP]);
            DrawKey(effect, &m_layout.m_keys[PAD_DPAD_DOWN]);
            DrawKey(effect, &m_layout.m_keys[PAD_DPAD_LEFT]);
            DrawKey(effect, &m_layout.m_keys[PAD_DPAD_RIGHT]);
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
        else if (m_layout.m_type == TYPE_MOUSE)
        {
            line = cfg->get_string("mouse_layout_w_h");
            m_layout.m_w = read_chain_int(line);
            m_layout.m_h = read_chain_int(line);

            // Stitching together config value identifiers (what a mess)

            std::string vals[] = { "lmb", "rmb", "mmb", "smb1", "smb2", "body" };
            unsigned char keys[] { KEY_LMB, KEY_RMB, KEY_MMB, KEY_SMB1, KEY_SMB2, 0 };

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
            m_layout.m_w = cfg->get_int("pad_w");
            m_layout.m_h = cfg->get_int("pad_h");
            m_pad_settings.m_analog_radius = cfg->get_int("pad_analog_radius");

            InputKey keys[PAD_KEY_COUNT];
            keys[PAD_BODY].w = m_layout.m_w;
            keys[PAD_BODY].h = m_layout.m_h;
            keys[PAD_BODY].texture_u = 1;
            keys[PAD_BODY].texture_v = 1;

            //Sticks
            keys[PAD_L_ANALOG].texture_u = keys[PAD_R_ANALOG].texture_u = cfg->get_int("pad_analog_u");
            keys[PAD_L_ANALOG].texture_v = keys[PAD_R_ANALOG].texture_v = cfg->get_int("pad_analog_v");

            keys[PAD_L_ANALOG].column = cfg->get_int("pad_l_analog_x");
            keys[PAD_L_ANALOG].row = cfg->get_int("pad_l_analog_y");
            
            keys[PAD_R_ANALOG].column = cfg->get_int("pad_r_analog_x");
            keys[PAD_R_ANALOG].row = cfg->get_int("pad_r_analog_y");
            
            keys[PAD_L_ANALOG].h = keys[PAD_L_ANALOG].w = keys[PAD_R_ANALOG].h = keys[PAD_R_ANALOG].w = cfg->get_int("pad_analog_dim");

            // Start/Back
            keys[PAD_BACK].w = keys[PAD_START].w = cfg->get_int("pad_back_w");
            keys[PAD_BACK].h = keys[PAD_START].h = cfg->get_int("pad_back_h");

            keys[PAD_BACK].texture_u  = cfg->get_int("pad_back_u");
            keys[PAD_START].texture_v = keys[PAD_BACK].texture_v = cfg->get_int("pad_back_v");
            keys[PAD_START].texture_u = keys[PAD_BACK].texture_u + keys[PAD_BACK].w + 3;

            keys[PAD_BACK].column = cfg->get_int("pad_back_x");
            keys[PAD_BACK].row = cfg->get_int("pad_back_y");

            keys[PAD_START].column = cfg->get_int("pad_start_x");
            keys[PAD_START].row = cfg->get_int("pad_start_y");

            // Controller center button
            keys[PAD_PLAYER_1].column = keys[PAD_PLAYER_2].column =
                keys[PAD_PLAYER_3].column = keys[PAD_PLAYER_4].column = cfg->get_int("pad_port_x");
            keys[PAD_PLAYER_1].row = keys[PAD_PLAYER_2].row =
                keys[PAD_PLAYER_3].row = keys[PAD_PLAYER_4].row = cfg->get_int("pad_port_y");

            keys[PAD_PLAYER_1].w = keys[PAD_PLAYER_2].w = keys[PAD_PLAYER_3].w =
                keys[PAD_PLAYER_4].w = keys[PAD_PLAYER_1].h = keys[PAD_PLAYER_2].h =
                keys[PAD_PLAYER_3].h = keys[PAD_PLAYER_4].h = cfg->get_int("pad_port_dim");

            keys[PAD_PLAYER_1].texture_u = cfg->get_int("pad_port_u");
            keys[PAD_PLAYER_1].texture_v = keys[PAD_PLAYER_2].texture_v =
                keys[PAD_PLAYER_3].texture_v = keys[PAD_PLAYER_4].texture_v = cfg->get_int("pad_port_v");

            keys[PAD_PLAYER_2].texture_u = keys[PAD_PLAYER_1].texture_u + 3 + keys[PAD_PLAYER_1].w;
            keys[PAD_PLAYER_3].texture_u = keys[PAD_PLAYER_1].texture_u + (3 + keys[PAD_PLAYER_1].w) * 2;
            keys[PAD_PLAYER_4].texture_u = keys[PAD_PLAYER_1].texture_u + (3 + keys[PAD_PLAYER_1].w) * 3;

            // X, Y, A, B
            keys[PAD_A].texture_v = keys[PAD_B].texture_v = keys[PAD_Y].texture_v 
                = keys[PAD_X].texture_v = cfg->get_int("pad_x_v");
            keys[PAD_X].texture_u = cfg->get_int("pad_x_u");

            keys[PAD_X].w = keys[PAD_Y].w = keys[PAD_A].w = keys[PAD_B].w =
                keys[PAD_X].h = keys[PAD_Y].h = keys[PAD_A].h = keys[PAD_B].h = cfg->get_int("pad_x_dim");

            keys[PAD_Y].texture_u = keys[PAD_X].texture_u + 3 + keys[PAD_X].w;
            keys[PAD_B].texture_u = keys[PAD_X].texture_u + (3 + keys[PAD_X].w) * 2;
            keys[PAD_A].texture_u = keys[PAD_X].texture_u + (3 + keys[PAD_X].w) * 3;

            keys[PAD_X].column = cfg->get_int("pad_x_x");
            keys[PAD_X].row = cfg->get_int("pad_x_y");

            keys[PAD_Y].column = cfg->get_int("pad_y_x");
            keys[PAD_Y].row = cfg->get_int("pad_y_y");

            keys[PAD_A].column = cfg->get_int("pad_a_x");
            keys[PAD_A].row = cfg->get_int("pad_a_y");

            keys[PAD_B].column = cfg->get_int("pad_b_x");
            keys[PAD_B].row = cfg->get_int("pad_b_y");

            // Shoulder keys
            keys[PAD_LB].w = keys[PAD_RB].w = cfg->get_int("pad_b_w");
            keys[PAD_LB].h = keys[PAD_RB].h = cfg->get_int("pad_b_h");

            keys[PAD_LT].w = keys[PAD_RT].w = cfg->get_int("pad_t_w");
            keys[PAD_LT].h = keys[PAD_RT].h = cfg->get_int("pad_t_h");

            keys[PAD_LB].texture_u = cfg->get_int("pad_b_u");
            keys[PAD_RB].texture_u = keys[PAD_LB].texture_u + 3 + keys[PAD_LB].w;
            keys[PAD_LB].texture_v = keys[PAD_RB].texture_v = cfg->get_int("pad_b_v");

            keys[PAD_LT].texture_u = cfg->get_int("pad_t_u");
            keys[PAD_RT].texture_u = keys[PAD_LT].texture_u + 3 + keys[PAD_LT].w;
            keys[PAD_LT].texture_v = keys[PAD_RT].texture_v= cfg->get_int("pad_t_v");

            keys[PAD_LB].column = cfg->get_int("pad_lb_x");
            keys[PAD_LB].row = cfg->get_int("pad_lb_y");

            keys[PAD_LT].column = cfg->get_int("pad_lt_x");
            keys[PAD_LT].row = cfg->get_int("pad_lt_y");

            keys[PAD_RB].column = cfg->get_int("pad_rb_x");
            keys[PAD_RB].row = cfg->get_int("pad_rb_y");

            keys[PAD_RT].column = cfg->get_int("pad_rt_x");
            keys[PAD_RT].row = cfg->get_int("pad_rt_y");

            // Dpad
            keys[PAD_DPAD_UP].texture_u = keys[PAD_DPAD_DOWN].texture_u = keys[PAD_DPAD_LEFT].texture_u =
                keys[PAD_DPAD_RIGHT].texture_u = cfg->get_int("pad_dpad_pressed_u");
            keys[PAD_DPAD_UP].texture_v = keys[PAD_DPAD_DOWN].texture_v = keys[PAD_DPAD_LEFT].texture_v =
                keys[PAD_DPAD_RIGHT].texture_v = cfg->get_int("pad_dpad_pressed_v");

            keys[PAD_DPAD_UP].w = keys[PAD_DPAD_DOWN].w = keys[PAD_DPAD_LEFT].w = keys[PAD_DPAD_RIGHT].w =
                keys[PAD_DPAD_UP].h = keys[PAD_DPAD_DOWN].h = keys[PAD_DPAD_LEFT].h = keys[PAD_DPAD_RIGHT].h =
                cfg->get_int("pad_dpad_dim");

            keys[PAD_DPAD_UP].column = cfg->get_int("pad_dpad_up_x");
            keys[PAD_DPAD_UP].row = cfg->get_int("pad_dpad_up_y");
            keys[PAD_DPAD_DOWN].column = cfg->get_int("pad_dpad_down_x");
            keys[PAD_DPAD_DOWN].row = cfg->get_int("pad_dpad_down_y");
            keys[PAD_DPAD_LEFT].column = cfg->get_int("pad_dpad_left_x");
            keys[PAD_DPAD_LEFT].row = cfg->get_int("pad_dpad_left_y");
            keys[PAD_DPAD_RIGHT].column = cfg->get_int("pad_dpad_right_x");
            keys[PAD_DPAD_RIGHT].row = cfg->get_int("pad_dpad_right_y");

            for (int i = 0; i < PAD_KEY_COUNT; i++)
            {
                m_layout.m_keys.emplace_back(keys[i]);
            }
        }

        m_layout.m_key_count = min(m_layout.m_key_count, m_layout.m_keys.size());
        m_layout.m_is_loaded = true;

        cx = m_layout.m_w;
        cy = m_layout.m_h;
    }
  
    delete cfg;
}

void InputSource::UnloadOverlayLayout()
{
    if (!m_layout.m_keys.empty())
    {
        m_layout.m_keys.clear();
    }

    ZeroMemory(&m_xinput, sizeof(XINPUT_STATE));
}

void InputSource::CheckKeys()
{
    if (m_layout.m_is_loaded)
    {
        if (m_layout.m_type == TYPE_KEYBOARD || m_layout.m_type == TYPE_MOUSE)
        {
            for (int i = 0; i < m_layout.m_key_count; i++)
            {
                m_layout.m_keys[i].m_pressed = GetAsyncKeyState(m_layout.m_keys[i].m_key_char) & SHIFTED;
            }
        }
        else if (m_layout.m_type == TYPE_CONTROLLER)
        {
            ZeroMemory(&m_xinput, sizeof(XINPUT_STATE));
            m_valid_controller = false;
            if (XInputGetState(m_pad_settings.m_controller_id, &m_xinput) == ERROR_SUCCESS)
            {
                m_valid_controller = true;
            }

            if (m_valid_controller)
            {
                CheckGamePadKeys();
            }
        }
    }
}

void InputSource::CheckGamePadKeys(void)
{
    m_layout.m_keys[PAD_L_ANALOG].m_pressed = X_PRESSED(XINPUT_GAMEPAD_LEFT_THUMB);
    m_layout.m_keys[PAD_R_ANALOG].m_pressed = X_PRESSED(XINPUT_GAMEPAD_RIGHT_THUMB);

    m_layout.m_keys[PAD_BACK].m_pressed = X_PRESSED(XINPUT_GAMEPAD_BACK);
    m_layout.m_keys[PAD_START].m_pressed = X_PRESSED(XINPUT_GAMEPAD_START);

    m_layout.m_keys[PAD_X].m_pressed = X_PRESSED(XINPUT_GAMEPAD_X);
    m_layout.m_keys[PAD_Y].m_pressed = X_PRESSED(XINPUT_GAMEPAD_Y);
    m_layout.m_keys[PAD_A].m_pressed = X_PRESSED(XINPUT_GAMEPAD_A);
    m_layout.m_keys[PAD_B].m_pressed = X_PRESSED(XINPUT_GAMEPAD_B);

    m_layout.m_keys[PAD_LB].m_pressed = X_PRESSED(XINPUT_GAMEPAD_LEFT_SHOULDER);
    m_layout.m_keys[PAD_LT].m_pressed = m_xinput.Gamepad.bLeftTrigger > 20;

    m_layout.m_keys[PAD_RB].m_pressed = X_PRESSED(XINPUT_GAMEPAD_RIGHT_SHOULDER);
    m_layout.m_keys[PAD_RT].m_pressed = m_xinput.Gamepad.bRightTrigger > 20;

    m_layout.m_keys[PAD_DPAD_UP].m_pressed = X_PRESSED(XINPUT_GAMEPAD_DPAD_UP);
    m_layout.m_keys[PAD_DPAD_DOWN].m_pressed = X_PRESSED(XINPUT_GAMEPAD_DPAD_DOWN);
    m_layout.m_keys[PAD_DPAD_LEFT].m_pressed = X_PRESSED(XINPUT_GAMEPAD_DPAD_LEFT);
    m_layout.m_keys[PAD_DPAD_RIGHT].m_pressed = X_PRESSED(XINPUT_GAMEPAD_DPAD_RIGHT);

    if (!DEAD_ZONE(m_xinput.Gamepad.sThumbLX, m_pad_settings.m_left_dead_zone))
    {
        m_pad_settings.m_left_analog_x = fmaxf(-1, (float)m_xinput.Gamepad.sThumbLX / PAD_STICK_MAX_VAL);
    }
    else
    {
        m_pad_settings.m_left_analog_x = 0.f;
    }

    if (!DEAD_ZONE(m_xinput.Gamepad.sThumbLY, m_pad_settings.m_left_dead_zone))
    {
        m_pad_settings.m_left_analog_y = fmaxf(-1, (float)m_xinput.Gamepad.sThumbLY / PAD_STICK_MAX_VAL);
    }
    else
    {
        m_pad_settings.m_left_analog_y = 0.f;
    }

    if (!DEAD_ZONE(m_xinput.Gamepad.sThumbRX, m_pad_settings.m_right_dead_zone))
    {
        m_pad_settings.m_right_analog_x = fmaxf(-1, (float)m_xinput.Gamepad.sThumbRX / PAD_STICK_MAX_VAL);
    }
    else
    {
        m_pad_settings.m_right_analog_x = 0.f;
    }

    if (!DEAD_ZONE(m_xinput.Gamepad.sThumbRY, m_pad_settings.m_right_dead_zone))
    {
        m_pad_settings.m_right_analog_y = fmaxf(-1, (float)m_xinput.Gamepad.sThumbRY / PAD_STICK_MAX_VAL);
    }
    else
    {
        m_pad_settings.m_right_analog_y = 0.f;
    }
}

bool is_controller_changed(obs_properties_t * props, obs_property_t * p, obs_data_t * s)
{
    bool is_gamepad = obs_data_get_bool(s, S_IS_CONTROLLER);
    
    obs_property_t *id = obs_properties_get(props, S_CONTROLLER_ID);
    obs_property_t *l_deadzone = obs_properties_get(props, S_CONTROLLER_L_DEAD_ZONE);
    obs_property_t *r_deadzone = obs_properties_get(props, S_CONTROLLER_R_DEAD_ZONE);

    obs_property_set_visible(id, is_gamepad);
    obs_property_set_visible(l_deadzone, is_gamepad);
    obs_property_set_visible(r_deadzone, is_gamepad);

    return true;
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

    // Gamepad stuff

    obs_property_t *p = obs_properties_add_bool(props, S_IS_CONTROLLER, T_IS_CONTROLLER);
    obs_property_set_modified_callback(p, is_controller_changed);

    obs_properties_add_int(props, S_CONTROLLER_ID, T_CONTROLLER_ID, 0, 3, 1);

    obs_properties_add_int_slider(props, S_CONTROLLER_L_DEAD_ZONE, 
        T_CONROLLER_L_DEADZONE, 1, PAD_STICK_MAX_VAL - 1, 1);
    obs_properties_add_int_slider(props, S_CONTROLLER_R_DEAD_ZONE, 
        T_CONROLLER_R_DEADZONE, 1, PAD_STICK_MAX_VAL - 1, 1);

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