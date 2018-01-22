#include "input-source.hpp"

uint16_t pressed_keys[MAX_SIMULTANEOUS_KEYS];
bool hook_initialized = false;

#ifdef WINDOWS
static HANDLE hook_thread;
static HANDLE hook_running_mutex;
static HANDLE hook_control_mutex;
static HANDLE hook_control_cond;
#else
static pthread_t hook_thread;
static pthread_mutex_t hook_running_mutex;
static pthread_mutex_t hook_control_mutex;
static pthread_cond_t hook_control_cond;
#endif

void InputSource::draw_key(gs_effect_t *effect, InputKey* key, uint16_t x, uint16_t y)
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
void InputSource::draw_key(gs_effect_t * effect, InputKey * key)
{
    draw_key(effect, key, key->column, key->row);
}

void InputSource::unload_texture()
{
    obs_enter_graphics();
    gs_image_file_free(m_image);
    obs_leave_graphics();
}

inline void InputSource::Update(obs_data_t *settings)
{
    m_pad_settings.m_controller_id = obs_data_get_int(settings, S_CONTROLLER_ID);
    m_pad_settings.m_left_dead_zone = obs_data_get_int(settings, S_CONTROLLER_L_DEAD_ZONE);
    m_pad_settings.m_right_dead_zone = obs_data_get_int(settings, S_CONTROLLER_R_DEAD_ZONE);
    
    m_image_file = obs_data_get_string(settings, S_OVERLAY_FILE);
    m_layout_file = obs_data_get_string(settings, S_LAYOUT_FILE);

    load_texture();
    load_layout();

    if (!m_layout.m_is_loaded && m_image && m_image->texture)
    {
        cx = m_image->cx;
        cy = m_image->cy;
    }
}

inline void InputSource::Tick(float seconds)
{
    check_keys();
}

inline void InputSource::Render(gs_effect_t *effect)
{
    if (!m_image || !m_image->texture)
        return;

    if (m_layout_file.empty() || !m_layout.m_is_loaded)
    {
        gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), m_image->texture);
        gs_draw_sprite(m_image->texture, 0, cx, cy);
    }
    else
    {
        int x = 0, y = 0;
        InputKey k;

        if (m_layout.m_type == TYPE_KEYBOARD)
        {
            for (int i = 0; i < m_layout.m_key_count; i++)
            {
                k = m_layout.m_keys[i];
                x = m_layout.m_btn_w * k.column + (m_layout.m_key_space_h * k.column) + k.x_offset;
                y = m_layout.m_btn_h * k.row + (m_layout.m_key_space_v * k.row);
                draw_key(effect, &k, x, y);
            }
        }
        else if (m_layout.m_type == TYPE_MOUSE)
        {
            for (int i = 0; i < m_layout.m_key_count; i++)
            {
                k = m_layout.m_keys[i];
                draw_key(effect, &k);
            }
        }
        else if (m_layout.m_type == TYPE_CONTROLLER)
        {
            draw_key(effect, &m_layout.m_keys[PAD_LT]);
            draw_key(effect, &m_layout.m_keys[PAD_RT]);
            draw_key(effect, &m_layout.m_keys[PAD_RB]);
            draw_key(effect, &m_layout.m_keys[PAD_LB]);

            draw_key(effect, &m_layout.m_keys[PAD_BODY], 0, 0);
            
            InputKey k = m_layout.m_keys[PAD_L_ANALOG];
            x = k.column - k.w / 2 + m_pad_settings.m_analog_radius * m_pad_settings.m_left_analog_x;
            y = k.row - k.h / 2 - m_pad_settings.m_analog_radius * m_pad_settings.m_left_analog_y;
            draw_key(effect, &k, x, y);

            k = m_layout.m_keys[PAD_R_ANALOG];
            x = k.column - k.w / 2 + m_pad_settings.m_analog_radius * m_pad_settings.m_right_analog_x;
            y = k.row - k.h / 2 - m_pad_settings.m_analog_radius * m_pad_settings.m_right_analog_y;
            draw_key(effect, &k, x, y);

            draw_key(effect, &m_layout.m_keys[PAD_BACK]);
            draw_key(effect, &m_layout.m_keys[PAD_START]);
            draw_key(effect, &m_layout.m_keys[PAD_PLAYER_1 + m_pad_settings.m_controller_id]);
            draw_key(effect, &m_layout.m_keys[PAD_X]);
            draw_key(effect, &m_layout.m_keys[PAD_Y]);
            draw_key(effect, &m_layout.m_keys[PAD_A]);
            draw_key(effect, &m_layout.m_keys[PAD_B]);
            draw_key(effect, &m_layout.m_keys[PAD_DPAD_UP]);
            draw_key(effect, &m_layout.m_keys[PAD_DPAD_DOWN]);
            draw_key(effect, &m_layout.m_keys[PAD_DPAD_LEFT]);
            draw_key(effect, &m_layout.m_keys[PAD_DPAD_RIGHT]);
        }
    }
}

void InputSource::load_texture()
{
    unload_texture();

    if (!m_image_file.empty())
    {
        if (m_image == nullptr)
        {
            m_image = new gs_image_file_t();
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

void InputSource::load_layout()
{
    m_layout.m_is_loaded = false;

    if (m_layout_file.empty())
        return;

    unload_layout();

    std::string line;
    std::string key_order, key_width, key_height, key_col, key_row;

    ccl_config *cfg = new ccl_config(m_layout_file, "");

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
                k.m_key_code = read_chain(key_order);
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

            std::string vals[] = { "lmb", "rmb", "mmb", "smb1", "smb2", "mwu", "mwd", "body" };
            uint16_t keys[] { VC_MOUSE_BUTTON1, VC_MOUSE_BUTTON2, VC_MOUSE_BUTTON3, VC_MOUSE_BUTTON5,
                VC_MOUSE_BUTTON4, VC_MOUSE_WHEEL_UP, VC_MOUSE_WHEEL_DOWN, 0xffff};

            std::stringstream stream;
            std::string begin;
            
            for (int i = 0; i < 8; i++)
            {
                InputKey m;
                m.m_key_code = keys[i];

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

        m_layout.m_key_count = UTIL_MIN(m_layout.m_key_count, m_layout.m_keys.size());
        m_layout.m_is_loaded = true;
        
        cx = m_layout.m_w;
        cy = m_layout.m_h;
    }
    else
    {
        warning("Failed to load config! Not a valid config file\n");
    }
  
    delete cfg;
}

void InputSource::unload_layout()
{
    if (!m_layout.m_keys.empty())
    {
        m_layout.m_keys.clear();
    }

#if HAVE_XINPUT
    ZeroMemory(&m_xinput, sizeof(XINPUT_STATE));
#endif
}

void InputSource::check_keys()
{
    if (m_layout.m_is_loaded)
    {
        if (m_layout.m_type == TYPE_KEYBOARD || m_layout.m_type == TYPE_MOUSE)
        {
            for (int i = 0; i < m_layout.m_key_count; i++)
            {
                m_layout.m_keys[i].m_pressed = false;
            }
            
            if (!util_pressed_empty())
            {
                for (int i = 0; i < m_layout.m_key_count; i++)
                {
                    if (util_key_exists(m_layout.m_keys[i].m_key_code))
                    {
                        m_layout.m_keys[i].m_pressed = true;
                    }
                }
            }
        }
        else if (m_layout.m_type == TYPE_CONTROLLER)
        {
#if HAVE_XINPUT
            ZeroMemory(&m_xinput, sizeof(XINPUT_STATE));
            m_valid_controller = false;
            if (XInputGetState(m_pad_settings.m_controller_id, &m_xinput) == ERROR_SUCCESS)
            {
                m_valid_controller = true;
            }

            if (m_valid_controller)
            {
                check_gamepad();
            }
#endif
        }
    }
}

void InputSource::check_gamepad(void)
{
#if HAVE_XINPUT
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
#endif
}

bool util_pressed_empty(void)
{
    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
    {
        if (pressed_keys[i] > 0)
            return false;
    }
    return true;
}

bool util_key_exists(uint16_t vc)
{
    if (util_pressed_empty())
        return false;
    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
    {
        if (pressed_keys[i] == vc)
            return true;
    }
    return false;
}

void util_add_pressed(uint16_t vc)
{
    if (!util_key_exists(vc))
    {
        int next_free = -1;
        for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
        {
            if (pressed_keys[i] == 0)
            {
                next_free = i;
                break;
            }
        }

        if (next_free > -1)
        {
            pressed_keys[next_free] = vc;
        }
    }
}

void util_remove_pressed(uint16_t vc)
{
    if (util_key_exists(vc))
    {
        for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
        {
            if (pressed_keys[i] == vc)
            {
                pressed_keys[i] = VC_UNDEFINED;
                break;
            }
        }
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

    if (s && !s->m_image_file.empty())
    {
        const char *slash;

        img_path = s->m_image_file;
        replace(img_path.begin(), img_path.end(), '\\', '/');
        slash = strrchr(img_path.c_str(), '/');
        if (slash)
            img_path.resize(slash - img_path.c_str() + 1);
    }

    if (s && !s->m_layout_file.empty())
    {
        const char *slash;
        layout_path = s->m_layout_file;
        replace(layout_path.begin(), layout_path.end(), '\\', '/');
        slash = strrchr(layout_path.c_str(), '/');
        if (slash)
            layout_path.resize(slash - layout_path.c_str() + 1);
    }

    obs_properties_add_path(props, S_OVERLAY_FILE, T_OVERLAY_FILE, OBS_PATH_FILE,
        filter_img.c_str(), img_path.c_str());

    obs_properties_add_path(props, S_LAYOUT_FILE, T_LAYOUT_FILE, OBS_PATH_FILE,
        filter_text.c_str(), layout_path.c_str());

    // Gamepad stuff
#ifdef HAVE_XINPUT
    obs_property_t *p = obs_properties_add_bool(props, S_IS_CONTROLLER, T_IS_CONTROLLER);
    obs_property_set_modified_callback(p, is_controller_changed);

    obs_properties_add_int(props, S_CONTROLLER_ID, T_CONTROLLER_ID, 0, 3, 1);

    obs_properties_add_int_slider(props, S_CONTROLLER_L_DEAD_ZONE, 
        T_CONROLLER_L_DEADZONE, 1, PAD_STICK_MAX_VAL - 1, 1);
    obs_properties_add_int_slider(props, S_CONTROLLER_R_DEAD_ZONE, 
        T_CONROLLER_R_DEADZONE, 1, PAD_STICK_MAX_VAL - 1, 1);
#endif
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
        // NO-OP
    };

    si.update = [](void *data, obs_data_t *settings) { reinterpret_cast<InputSource*>(data)->Update(settings); };
    si.video_tick = [](void *data, float seconds) { reinterpret_cast<InputSource*>(data)->Tick(seconds); };
    si.video_render = [](void *data, gs_effect_t *effect) { reinterpret_cast<InputSource*>(data)->Render(effect); };
    obs_register_source(&si);
}

uint16_t mouse_to_vc(int m)
{
    return (uint16_t) (VC_MOUSE_MASK | m);
}

/* ------ LIBUIOHOOK ------*/

void dispatch_proc(uiohook_event * const event)
{
    switch (event->type)
    {
    case EVENT_HOOK_ENABLED:
#ifdef WINDOWS
        WaitForSingleObject(hook_running_mutex, INFINITE);
#else
        pthread_mutex_lock(&hook_running_mutex);
#endif

#ifdef WINDOWS
        SetEvent(hook_control_cond);
#else
        pthread_cond_signal(&hook_control_cond);
        pthread_mutex_unlock(&hook_control_mutex);
#endif
        break;
    case EVENT_HOOK_DISABLED:
#ifdef WINDOWS
        WaitForSingleObject(hook_control_mutex, INFINITE);
#else
        pthread_mutex_lock(&hook_control_mutex);
#endif

#ifdef WINDOWS
        ReleaseMutex(hook_running_mutex);
        ResetEvent(hook_control_cond);
#else
#if defined(__APPLE__) && defined(__MACH__)
        CFRunLoopStop(CFRunLoopGetMain());
#endif
        pthread_mutex_unlock(&hook_running_mutex);
#endif
    }

    proccess_event(event);
}

#ifdef WINDOWS
DWORD WINAPI hook_thread_proc(LPVOID arg)
{
    // Set the hook status.
    int status = hook_run();
    if (status != UIOHOOK_SUCCESS) {
        *(DWORD *)arg = status;
        *(int *)arg = status;
    }

    SetEvent(hook_control_cond);
    return status;
}
#else
void *hook_thread_proc(void *arg)
{
    int status = hook_run();
    if (status != UIOHOOK_SUCCESS) {
        *(int *)arg = status;
    }
    pthread_cond_signal(&hook_control_cond);
    pthread_mutex_unlock(&hook_control_mutex);

    return arg;
}
#endif

bool logger_proc(unsigned int level, const char * format, ...)
{
    va_list args;
    switch (level)
    {
    case LOG_LEVEL_WARN:
    case LOG_LEVEL_ERROR:
        va_start(args, format);
        std::string f(format);
        f.insert(0, "[input-overlay] ");
        blog(LOG_WARNING, f.c_str(), args);
        va_end(args);
    }
    return true;
}

void start_hook(void)
{
#ifdef _WIN32
    hook_running_mutex = CreateMutex(NULL, FALSE, TEXT("hook_running_mutex"));
    hook_control_mutex = CreateMutex(NULL, FALSE, TEXT("hook_control_mutex"));
    hook_control_cond = CreateEvent(NULL, TRUE, FALSE, TEXT("hook_control_cond"));
#else
    pthread_mutex_init(&hook_running_mutex, NULL);
    pthread_mutex_init(&hook_control_mutex, NULL);
    pthread_cond_init(&hook_control_cond, NULL);
#endif

    // Set the logger callback for library output.
    hook_set_logger_proc(&logger_proc);

    // Set the event callback for uiohook events.
    hook_set_dispatch_proc(&dispatch_proc);

    int status = hook_enable();
    switch (status) {
    case UIOHOOK_SUCCESS:
        // We no longer block, so we need to explicitly wait for the thread to die.
        hook_initialized = true;
        break;
    case UIOHOOK_ERROR_OUT_OF_MEMORY:
        blog(LOG_ERROR, "[input-overlay] Failed to allocate memory. (%#X)\n", status);
        break;
    case UIOHOOK_ERROR_X_OPEN_DISPLAY:
        blog(LOG_ERROR, "[input-overlay] Failed to open X11 display. (%#X)\n", status);
        break;
    case UIOHOOK_ERROR_X_RECORD_NOT_FOUND:
        blog(LOG_ERROR, "[input-overlay] Unable to locate XRecord extension. (%#X)\n", status);
        break;
    case UIOHOOK_ERROR_X_RECORD_ALLOC_RANGE:
        blog(LOG_ERROR, "[input-overlay] Unable to allocate XRecord range. (%#X)\n", status);
        break;
    case UIOHOOK_ERROR_X_RECORD_CREATE_CONTEXT:
        blog(LOG_ERROR, "[input-overlay] Unable to allocate XRecord context. (%#X)\n", status);
        break;
    case UIOHOOK_ERROR_X_RECORD_ENABLE_CONTEXT:
        blog(LOG_ERROR, "[input-overlay] Failed to enable XRecord context. (%#X)\n", status);
        break;
    case UIOHOOK_ERROR_SET_WINDOWS_HOOK_EX:
        blog(LOG_ERROR, "[input-overlay] Failed to register low level windows hook. (%#X)\n", status);
        break;
    case UIOHOOK_ERROR_CREATE_EVENT_PORT:
        blog(LOG_ERROR, "[input-overlay] Failed to create apple event port. (%#X)\n", status);
        break;
    case UIOHOOK_ERROR_CREATE_RUN_LOOP_SOURCE:
        blog(LOG_ERROR, "[input-overlay] Failed to create apple run loop source. (%#X)\n", status);
        break;
    case UIOHOOK_ERROR_GET_RUNLOOP:
        blog(LOG_ERROR, "[input-overlay] Failed to acquire apple run loop. (%#X)\n", status);
        break;
    case UIOHOOK_ERROR_CREATE_OBSERVER:
        blog(LOG_ERROR, "[input-overlay] Failed to create apple run loop observer. (%#X)\n", status);
        break;
    case UIOHOOK_FAILURE:
    default:
        blog(LOG_ERROR, "[input-overlay] An unknown hook error occurred. (%#X)\n", status);
        break;
    }
}

void end_hook(void)
{
#ifdef WINDOWS
    // Create event handles for the thread hook.
    CloseHandle(hook_thread);
    CloseHandle(hook_running_mutex);
    CloseHandle(hook_control_mutex);
    CloseHandle(hook_control_cond);
#else
    pthread_mutex_destroy(&hook_running_mutex);
    pthread_mutex_destroy(&hook_control_mutex);
    pthread_cond_destroy(&hook_control_cond);
#endif
    hook_stop();
}

void proccess_event(uiohook_event * const event)
{
    util_remove_pressed(VC_MOUSE_WHEEL_UP);
    util_remove_pressed(VC_MOUSE_WHEEL_DOWN);
    
    switch (event->type)
    {
        case EVENT_KEY_PRESSED:
            if (!util_key_exists(event->data.keyboard.keycode))
                util_add_pressed(event->data.keyboard.keycode);
            break;
        case EVENT_KEY_RELEASED:
            if (util_key_exists(event->data.keyboard.keycode))
                util_remove_pressed(event->data.keyboard.keycode);
                break;
        case EVENT_MOUSE_PRESSED:
            if (!util_key_exists(mouse_to_vc(event->data.keyboard.keycode)))
                util_add_pressed(mouse_to_vc(event->data.keyboard.keycode));
            break;
        case EVENT_MOUSE_RELEASED:
            if (util_key_exists(mouse_to_vc(event->data.mouse.button)))
                util_remove_pressed(mouse_to_vc(event->data.mouse.button));
            break;
        case EVENT_MOUSE_WHEEL:
            if (event->data.wheel.rotation == WHEEL_UP)
            {
                if (!util_key_exists(VC_MOUSE_WHEEL_UP))
                {
                    util_add_pressed(VC_MOUSE_WHEEL_UP);
                }
            }
            else if (event->data.wheel.rotation == WHEEL_DOWN)
            {
                if (!util_key_exists(VC_MOUSE_WHEEL_DOWN))
                {
                    util_add_pressed(VC_MOUSE_WHEEL_DOWN);
                }
            }
            break;
    }
}

int hook_enable(void)
{
    // Lock the thread control mutex.  This will be unlocked when the
    // thread has finished starting, or when it has fully stopped.
#ifdef WINDOWS
    WaitForSingleObject(hook_control_mutex, INFINITE);
#else
    pthread_mutex_lock(&hook_control_mutex);
#endif

    // Set the initial status.
    int status = UIOHOOK_FAILURE;

#ifndef WINDOWS
    // Create the thread attribute.
    pthread_attr_t hook_thread_attr;
    pthread_attr_init(&hook_thread_attr);

    // Get the policy and priority for the thread attr.
    int policy;
    pthread_attr_getschedpolicy(&hook_thread_attr, &policy);
    int priority = sched_get_priority_max(policy);
#endif

#if defined(WINDOWS)
    DWORD hook_thread_id;
    DWORD *hook_thread_status = (DWORD*) malloc(sizeof(DWORD));
    hook_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hook_thread_proc, hook_thread_status, 0, &hook_thread_id);
    if (hook_thread != INVALID_HANDLE_VALUE) {
#else
    int *hook_thread_status = (int*) malloc(sizeof(int));
    if (pthread_create(&hook_thread, &hook_thread_attr, hook_thread_proc, hook_thread_status) == 0) {
#endif
#if defined(WINDOWS)
        // Attempt to set the thread priority to time critical.
        if (SetThreadPriority(hook_thread, THREAD_PRIORITY_TIME_CRITICAL) == 0) {
            blog(LOG_WARNING, "[input-overlay] %s [%u]: Could not set thread priority %li for hook thread %#p! (%#lX)\n",
                __FUNCTION__, __LINE__, (long) THREAD_PRIORITY_TIME_CRITICAL, hook_thread, (unsigned long) GetLastError());
        }
#elif (defined(__APPLE__) && defined(__MACH__)) || _POSIX_C_SOURCE >= 200112L
        // Some POSIX revisions do not support pthread_setschedprio so we will 
        // use pthread_setschedparam instead.
        struct sched_param param = { .sched_priority = priority };
        if (pthread_setschedparam(hook_thread, SCHED_OTHER, &param) != 0) {
            blog(LOG_WARNING, "[input-overlay] %s [%u]: Could not set thread priority %i for thread 0x%lX!\n",
                __FUNCTION__, __LINE__, priority, (unsigned long)hook_thread);
    }
#else
        // Raise the thread priority using glibc pthread_setschedprio.
        if (pthread_setschedprio(hook_thread, priority) != 0) {
            blog(LOG_WARNING, "[input-overlay] %s [%u]: Could not set thread priority %i for thread 0x%lX!\n",
                __FUNCTION__, __LINE__, priority, (unsigned long)hook_thread);
        }
#endif
        // Wait for the thread to indicate that it has passed the 
        // initialization portion by blocking until either a EVENT_HOOK_ENABLED 
        // event is received or the thread terminates.
        // NOTE This unlocks the hook_control_mutex while we wait.
#ifdef WINDOWS
        WaitForSingleObject(hook_control_cond, INFINITE);
#else
        pthread_cond_wait(&hook_control_cond, &hook_control_mutex);
#endif

#ifdef WINDOWS
        if (WaitForSingleObject(hook_running_mutex, 0) != WAIT_TIMEOUT) {
#else
        if (pthread_mutex_trylock(&hook_running_mutex) == 0) {
#endif
            // Lock Successful; The hook is not running but the hook_control_cond 
            // was signaled!  This indicates that there was a startup problem!

            // Get the status back from the thread.
#ifdef WINDOWS
            WaitForSingleObject(hook_thread, INFINITE);
            GetExitCodeThread(hook_thread, hook_thread_status);
#else
            pthread_join(hook_thread, (void **)&hook_thread_status);
            status = *hook_thread_status;
#endif
        }
        else {
            // Lock Failure; The hook is currently running and wait was signaled
            // indicating that we have passed all possible start checks.  We can 
            // always assume a successful startup at this point.
            status = UIOHOOK_SUCCESS;
        }

        free(hook_thread_status);

        blog(LOG_DEBUG, "[input-overlay] %s [%u]: Thread Result: (%#X).\n",
            __FUNCTION__, __LINE__, status);
    }
    else
    {
        status = -1;
    }

    // Make sure the control mutex is unlocked.
#ifdef WINDOWS
    ReleaseMutex(hook_control_mutex);
#else
    pthread_mutex_unlock(&hook_control_mutex);
#endif

    return status;
}
