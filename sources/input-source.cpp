#include "input-source.hpp"

/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

void InputSource::draw_key(gs_effect_t *effect, InputKey* key, uint16_t x, uint16_t y, bool rot, float angle)
{
    gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), m_image->texture);
    
    gs_matrix_push();
    
    if (rot)
    {
        gs_matrix_translate3f(x + (key->w / 2.f),  y + (key->h / 2.f), 1.f); // Put it in position
        gs_matrix_translate3f((float)-(key->w / 2), (float)-(key->h / 2), 1.f); // Needed for rotation
        gs_matrix_rotaa4f(0.f, 0.f, 1.f, angle);
        gs_matrix_translate3f((float)-(key->w / 2), (float)-(key->h / 2), 1.f); // Needed for rotation
        gs_draw_sprite_subregion(m_image->texture, 0, key->texture_u, key->texture_v, key->w + 1, key->h + 1);
    }
    else
    {
        gs_matrix_translate3f((float)x, (float)y, 1.f);

        if (key->m_pressed)
        {
            gs_draw_sprite_subregion(m_image->texture, 0, key->texture_u, key->texture_v + key->h + 3, key->w + 1, key->h + 1);
        }
        else
        {
            gs_draw_sprite_subregion(m_image->texture, 0, key->texture_u, key->texture_v, key->w + 1, key->h + 1);
        }
    }

    gs_matrix_pop();
}

void InputSource::draw_key(gs_effect_t * effect, InputKey * key, uint16_t x, uint16_t y)
{
    draw_key(effect, key, x, y, false, 0.f);
}

void InputSource::draw_key(gs_effect_t * effect, InputKey * key)
{
    draw_key(effect, key, key->column, key->row, false, 0.f);
}

void InputSource::unload_texture()
{
    obs_enter_graphics();
    gs_image_file_free(m_image);
    obs_leave_graphics();
}

inline void InputSource::Update(obs_data_t *settings)
{
    m_is_controller = obs_data_get_bool(settings, S_IS_CONTROLLER);

    if (m_is_controller)
    {
        uint8_t id = (uint8_t) obs_data_get_int(settings, S_CONTROLLER_ID);
        uint16_t l_dz = (uint16_t) obs_data_get_int(settings, S_CONTROLLER_L_DEAD_ZONE);
        uint16_t r_dz = (uint16_t) obs_data_get_int(settings, S_CONTROLLER_R_DEAD_ZONE);

        if (!m_gamepad)
        {
#ifdef HAVE_XINPUT
            m_gamepad = new WindowsGamepad(id, &m_layout.m_keys);
#endif

#ifdef LINUX_INPUT
            m_gamepad = new LinuxGamepad(id, &m_layout.m_keys);
#endif
        }

        m_gamepad->update(id, r_dz, l_dz);
    }
    
    m_layout.m_max_mouse_movement = (uint16_t) obs_data_get_int(settings, S_MOUSE_SENS);

    m_image_file = obs_data_get_string(settings, S_OVERLAY_FILE);
    m_layout_file = obs_data_get_string(settings, S_LAYOUT_FILE);

    m_monitor_use_center = obs_data_get_bool(settings, S_MONITOR_USE_CENTER);
    m_mouse_dead_zone = (uint8_t) obs_data_get_int(settings, S_MOUSE_DEAD_ZONE);

    if (m_monitor_use_center)
    {
        m_monitor_h = (uint32_t) obs_data_get_int(settings, S_MONITOR_H_CENTER);
        m_monitor_v = (uint32_t) obs_data_get_int(settings, S_MONITOR_V_CENTER);
    }

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
        uint16_t x = 0, y = 0;
        InputKey* k;

        if (m_layout.m_type == TYPE_KEYBOARD)
        {
            for (int i = 0; i < m_layout.m_key_count; i++)
            {
                k = &m_layout.m_keys[i];
                x = m_layout.m_btn_w * k->column + (m_layout.m_key_space_h * k->column) + k->x_offset;
                y = m_layout.m_btn_h * k->row + (m_layout.m_key_space_v * k->row);
                draw_key(effect, k, x, y);
            }

	    
        }
        else if (m_layout.m_type == TYPE_MOUSE)
        {
            int max = m_layout.m_mouse_movement ? m_layout.m_key_count - 1 : m_layout.m_key_count;

            for (int i = 0; i < max; i++)
            {
                k = &m_layout.m_keys[i];
                draw_key(effect, k);
            }

            if (m_layout.m_mouse_movement)
            {
                int d_x = mouse_x - (m_monitor_use_center ? m_monitor_h : mouse_last_x);
                int d_y = mouse_y - (m_monitor_use_center ? m_monitor_v : mouse_last_y);

                if (m_layout.m_use_arrow)
                {
                    k = &m_layout.m_keys[max];
                    float new_angle = (0.5 * M_PI) + (atan2f(d_y, d_x));
                    
                    if (abs(d_x) < m_mouse_dead_zone || abs(d_y) < m_mouse_dead_zone)
                    {
                        draw_key(effect, k, k->column, k->row, true, m_old_angle);
                    }
                    else
                    {
                       
                        draw_key(effect, k, k->column, k->row, true, new_angle);
                        m_old_angle = new_angle;
                    }
                }
                else
                {
                    double factor_x = UTIL_CLAMP(-1, ((double) d_x / m_layout.m_max_mouse_movement), 1); 
                    double factor_y = UTIL_CLAMP(-1, ((double) d_y / m_layout.m_max_mouse_movement), 1);

                    int16_t dot_offset_x = (uint16_t) (m_layout.m_track_radius * factor_x);
                    int16_t dot_offset_y = (uint16_t) (m_layout.m_track_radius * factor_y);

                    k = &m_layout.m_keys[max];
                    draw_key(effect, k, k->column + dot_offset_x, k->row + dot_offset_y);
                }
            }
        }
        else if (m_layout.m_type == TYPE_CONTROLLER)
        {
            /* Body is background */
            draw_key(effect, &m_layout.m_keys[PAD_BODY], 0, 0);

            if (m_gamepad)
            {
                /* Calculates position of analog sticks */
                k = &m_layout.m_keys[PAD_L_ANALOG];
                x = (uint16_t) (k->column - k->w / 2 + m_layout.m_track_radius *
                    m_gamepad->left_stick_x());
                y = (uint16_t) (k->row - k->h / 2 - m_layout.m_track_radius *
                    m_gamepad->left_stick_y());

                draw_key(effect, k, x, y);

                k = &m_layout.m_keys[PAD_R_ANALOG];
                x = (uint16_t) (k->column - k->w / 2 +  m_layout.m_track_radius *
                    m_gamepad->right_stick_x());
                y = (uint16_t) (k->row - k->h / 2 - m_layout.m_track_radius *
                    m_gamepad->right_stick_y());
                draw_key(effect, k, x, y);
                draw_key(effect, &m_layout.m_keys[PAD_PLAYER_1 + m_gamepad->get_id()]);
            }

            /* Draws rest of keys*/
            for (int i = 0; i < PAD_BUTTON_COUNT; i++)
            {
                if (i == PAD_R_ANALOG || i == PAD_L_ANALOG)
                    continue;
                draw_key(effect, &m_layout.m_keys[i]);
            }


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
   if (m_layout_file.empty())
        return;

    unload_layout();
    m_layout.m_is_loaded = false;

    std::string line;
    std::string key_order, key_width, key_height, key_col, key_row;
    int texture_w = 0;
    ccl_config *cfg = new ccl_config(m_layout_file, "");

    if (!cfg->is_empty())
    {
        m_layout.m_type = (LayoutType)cfg->get_int("layout_type");
        m_layout.m_key_count = (uint8_t) cfg->get_int("key_count");

        if (m_layout.m_type == TYPE_KEYBOARD) {
            m_layout.m_rows = (uint16_t) cfg->get_int("key_rows");
            m_layout.m_cols = (uint16_t) cfg->get_int("key_cols");
            m_layout.m_btn_w = (uint16_t) cfg->get_int("key_abs_w");
            m_layout.m_btn_h = (uint16_t) cfg->get_int("key_abs_h");
            m_layout.m_key_space_v = (int16_t) cfg->get_int("key_space_v");
            m_layout.m_key_space_h = (int16_t) cfg->get_int("key_space_h");
            m_layout.texture_v_space = (uint16_t)cfg->get_int("texture_v_space");
            
            key_order = cfg->get_string("key_order");
            key_width = cfg->get_string("key_width");
            key_height = cfg->get_string("key_height");
            key_col = cfg->get_string("key_col");
            key_row = cfg->get_string("key_row");
            texture_w = cfg->get_int("texture_w");
          
            uint16_t u_cord = 1, v_cord = 1;
            uint16_t tempw, temph;
            int index = 0;

            for (int i = 0; i < m_layout.m_key_count; i++)
            {
                if (index >= texture_w)
                {
                    index = 0;
                    u_cord = 1;
                    v_cord += m_layout.texture_v_space + 6;
                }

                InputKey k;
                k.texture_u = (uint16_t) (u_cord - 1);
                k.texture_v = (uint16_t) (v_cord - 1);
                tempw = util_read_int(key_width);
                temph = util_read_int(key_height);
		k.w = tempw * m_layout.m_btn_w + m_layout.m_key_space_h * (tempw - 1);
                k.h = temph * m_layout.m_btn_h + m_layout.m_key_space_v * (temph - 1);
                k.m_key_code = util_read_hex(key_order);
                k.m_pressed = false;
                k.row = util_read_int(key_row);
                k.column = util_read_int(key_col);

                if (tempw > 1 && m_layout.m_key_space_h > 0)
                {
                    k.x_offset = (uint16_t)
                            ((k.w + m_layout.m_key_space_h * (tempw - 1)) / 2 - k.w / 2);
		    k.w -= k.x_offset;
                    index += tempw;
                }
                else
                {
                    index++;
                    k.x_offset = 0;
                }

                m_layout.m_keys.emplace_back(k);
                u_cord += tempw * m_layout.m_btn_w + (tempw * 3);
            }

            m_layout.m_h = m_layout.m_rows * m_layout.m_btn_h + m_layout.m_key_space_v * m_layout.m_rows;
            m_layout.m_w = (m_layout.m_cols * m_layout.m_btn_w + m_layout.m_key_space_h *
		(m_layout.m_cols - 1));
        }
        else if (m_layout.m_type == TYPE_MOUSE)
        {
            m_layout.m_mouse_movement = cfg->get_bool("show_mouse_movement");
            m_layout.m_use_arrow = cfg->get_bool("use_arrow");
            m_layout.m_track_radius = cfg->get_int("mouse_field_radius");

            line = cfg->get_string("mouse_layout_w_h");
            m_layout.m_w = util_read_int(line);
            m_layout.m_h = util_read_int(line);

            // Stitching together config value identifiers (what a mess)

            std::string vals[] = { "lmb", "rmb", "mmb", "smb1", "smb2", "mwu", "mwd",
                "body", "arrow", "field", "dot" };
            uint16_t keys[] { VC_MOUSE_BUTTON1, VC_MOUSE_BUTTON2, VC_MOUSE_BUTTON3, VC_MOUSE_BUTTON5,
                VC_MOUSE_BUTTON4, VC_MOUSE_WHEEL_UP, VC_MOUSE_WHEEL_DOWN, VC_NONE, VC_NONE, VC_NONE, VC_NONE};

            std::stringstream stream;
            std::string begin;

            int max = (m_layout.m_mouse_movement && !m_layout.m_use_arrow) ? m_layout.m_key_count + 1 : m_layout.m_key_count; 

            for (int i = 0; i < max; i++)
            {
                if (!m_layout.m_mouse_movement && i > 7)
                    break;
                
                if (m_layout.m_mouse_movement)
                {
                    if (m_layout.m_use_arrow && i > 8)
                        break;
                    if (!m_layout.m_use_arrow && i == 8)
                        continue;
                }

                InputKey m;
                m.m_key_code = keys[i];

                stream.str("");
                begin = "mouse_" + vals[i];
                stream << begin << "_u_v";

                line = cfg->get_string(stream.str().c_str());
                m.texture_u = util_read_int(line);
                m.texture_v = util_read_int(line);

                stream.str("");
                stream << begin << "_w_h";
                
                line = cfg->get_string(stream.str().c_str());
                m.w = util_read_int(line);
                m.h = util_read_int(line);

                stream.str("");
                stream << begin << "_x_y";

                line = cfg->get_string(stream.str().c_str());
                m.column = util_read_int(line);
                m.row = util_read_int(line);

                m_layout.m_keys.emplace_back(m);
            }
            
        }
        else if (m_layout.m_type == TYPE_CONTROLLER)
        {
	   
            m_layout.m_w = (uint16_t) cfg->get_int("pad_w");
            m_layout.m_h = (uint16_t) cfg->get_int("pad_h");
            m_layout.m_track_radius = (uint16_t) cfg->get_int("pad_analog_radius");

            InputKey keys[PAD_ICON_COUNT];
            keys[PAD_BODY].w = m_layout.m_w;
            keys[PAD_BODY].h = m_layout.m_h;
            keys[PAD_BODY].texture_u = 1;
            keys[PAD_BODY].texture_v = 1;

            //Sticks
            keys[PAD_L_ANALOG].texture_u = keys[PAD_R_ANALOG].texture_u = (uint16_t) cfg->get_int("pad_analog_u");
            keys[PAD_L_ANALOG].texture_v = keys[PAD_R_ANALOG].texture_v = (uint16_t) cfg->get_int("pad_analog_v");

            keys[PAD_L_ANALOG].column = (uint16_t) cfg->get_int("pad_l_analog_x");
            keys[PAD_L_ANALOG].row = (uint16_t) cfg->get_int("pad_l_analog_y");
            
            keys[PAD_R_ANALOG].column = (uint16_t) cfg->get_int("pad_r_analog_x");
            keys[PAD_R_ANALOG].row = (uint16_t) cfg->get_int("pad_r_analog_y");
            
            keys[PAD_L_ANALOG].h = keys[PAD_L_ANALOG].w = keys[PAD_R_ANALOG].h = keys[PAD_R_ANALOG].w =
                    (uint16_t) cfg->get_int("pad_analog_dim");

            // Start/Back
            keys[PAD_BACK].w = keys[PAD_START].w = (uint16_t) cfg->get_int("pad_back_w");
            keys[PAD_BACK].h = keys[PAD_START].h = (uint16_t) cfg->get_int("pad_back_h");
           
            keys[PAD_BACK].texture_u  = (uint16_t) cfg->get_int("pad_back_u");

            keys[PAD_START].texture_v = keys[PAD_BACK].texture_v = (uint16_t) cfg->get_int("pad_back_v");
            keys[PAD_START].texture_u = (uint16_t) (keys[PAD_BACK].texture_u + keys[PAD_BACK].w + 3);

            keys[PAD_BACK].column = (uint16_t) cfg->get_int("pad_back_x");
            keys[PAD_BACK].row = (uint16_t) cfg->get_int("pad_back_y");

            keys[PAD_START].column = (uint16_t) cfg->get_int("pad_start_x");
            keys[PAD_START].row = (uint16_t) cfg->get_int("pad_start_y");

            // Controller center button
            keys[PAD_PLAYER_1].column = keys[PAD_PLAYER_2].column =
                keys[PAD_PLAYER_3].column = keys[PAD_PLAYER_4].column = (uint16_t) cfg->get_int("pad_port_x");
            keys[PAD_PLAYER_1].row = keys[PAD_PLAYER_2].row =
                keys[PAD_PLAYER_3].row = keys[PAD_PLAYER_4].row = (uint16_t) cfg->get_int("pad_port_y");

            keys[PAD_PLAYER_1].w = keys[PAD_PLAYER_2].w = keys[PAD_PLAYER_3].w =
                keys[PAD_PLAYER_4].w = keys[PAD_PLAYER_1].h = keys[PAD_PLAYER_2].h =
                keys[PAD_PLAYER_3].h = keys[PAD_PLAYER_4].h = (uint16_t) cfg->get_int("pad_port_dim");

            keys[PAD_PLAYER_1].texture_u = (uint16_t) cfg->get_int("pad_port_u");
            keys[PAD_PLAYER_1].texture_v = keys[PAD_PLAYER_2].texture_v =
                keys[PAD_PLAYER_3].texture_v = keys[PAD_PLAYER_4].texture_v = (uint16_t) cfg->get_int("pad_port_v");
            
            keys[PAD_PLAYER_2].texture_u = (uint16_t) (keys[PAD_PLAYER_1].texture_u + 3 + keys[PAD_PLAYER_1].w);
            keys[PAD_PLAYER_3].texture_u = (uint16_t) (keys[PAD_PLAYER_1].texture_u + (3 + keys[PAD_PLAYER_1].w) * 2);
            keys[PAD_PLAYER_4].texture_u = (uint16_t) (keys[PAD_PLAYER_1].texture_u + (3 + keys[PAD_PLAYER_1].w) * 3);

            // X, Y, A, B
            keys[PAD_A].texture_v = keys[PAD_B].texture_v = keys[PAD_Y].texture_v 
                = keys[PAD_X].texture_v = (uint16_t) cfg->get_int("pad_x_v");
            keys[PAD_X].texture_u = (uint16_t) cfg->get_int("pad_x_u");

            keys[PAD_X].w = keys[PAD_Y].w = keys[PAD_A].w = keys[PAD_B].w =
                keys[PAD_X].h = keys[PAD_Y].h = keys[PAD_A].h = keys[PAD_B].h = (uint16_t) cfg->get_int("pad_x_dim");

            keys[PAD_Y].texture_u = (uint16_t) (keys[PAD_X].texture_u + 3 + keys[PAD_X].w);
            keys[PAD_B].texture_u = (uint16_t) (keys[PAD_X].texture_u + (3 + keys[PAD_X].w) * 2);
            keys[PAD_A].texture_u = (uint16_t) (keys[PAD_X].texture_u + (3 + keys[PAD_X].w) * 3);

            keys[PAD_X].column = (uint16_t) cfg->get_int("pad_x_x");
            keys[PAD_X].row = (uint16_t) cfg->get_int("pad_x_y");

            keys[PAD_Y].column = (uint16_t) cfg->get_int("pad_y_x");
            keys[PAD_Y].row = (uint16_t)  cfg->get_int("pad_y_y");

            keys[PAD_A].column = (uint16_t) cfg->get_int("pad_a_x");
            keys[PAD_A].row = (uint16_t) cfg->get_int("pad_a_y");

            keys[PAD_B].column = (uint16_t) cfg->get_int("pad_b_x");
            keys[PAD_B].row = (uint16_t) cfg->get_int("pad_b_y");

            // Shoulder keys
            keys[PAD_LB].w = keys[PAD_RB].w = (uint16_t) cfg->get_int("pad_b_w");
            keys[PAD_LB].h = keys[PAD_RB].h = (uint16_t) cfg->get_int("pad_b_h");

            keys[PAD_LT].w = keys[PAD_RT].w = (uint16_t) cfg->get_int("pad_t_w");
            keys[PAD_LT].h = keys[PAD_RT].h = (uint16_t) cfg->get_int("pad_t_h");

            keys[PAD_LB].texture_u = (uint16_t) cfg->get_int("pad_b_u");
            keys[PAD_RB].texture_u = (uint16_t) (keys[PAD_LB].texture_u + 3 + keys[PAD_LB].w);
            keys[PAD_LB].texture_v = keys[PAD_RB].texture_v = (uint16_t) cfg->get_int("pad_b_v");

            keys[PAD_LT].texture_u = (uint16_t) cfg->get_int("pad_t_u");
            keys[PAD_RT].texture_u = (uint16_t) (keys[PAD_LT].texture_u + 3 + keys[PAD_LT].w);
            keys[PAD_LT].texture_v = keys[PAD_RT].texture_v = (uint16_t) cfg->get_int("pad_t_v");

            keys[PAD_LB].column = (uint16_t) cfg->get_int("pad_lb_x");
            keys[PAD_LB].row = (uint16_t) cfg->get_int("pad_lb_y");

            keys[PAD_LT].column = (uint16_t) cfg->get_int("pad_lt_x");
            keys[PAD_LT].row = (uint16_t) cfg->get_int("pad_lt_y");

            keys[PAD_RB].column = (uint16_t) cfg->get_int("pad_rb_x");
            keys[PAD_RB].row = (uint16_t) cfg->get_int("pad_rb_y");

            keys[PAD_RT].column = (uint16_t) cfg->get_int("pad_rt_x");
            keys[PAD_RT].row = (uint16_t) cfg->get_int("pad_rt_y");

            // Dpad
            keys[PAD_DPAD_UP].texture_u = keys[PAD_DPAD_DOWN].texture_u = keys[PAD_DPAD_LEFT].texture_u =
                keys[PAD_DPAD_RIGHT].texture_u = (uint16_t) cfg->get_int("pad_dpad_pressed_u");
            keys[PAD_DPAD_UP].texture_v = keys[PAD_DPAD_DOWN].texture_v = keys[PAD_DPAD_LEFT].texture_v =
                keys[PAD_DPAD_RIGHT].texture_v = (uint16_t) cfg->get_int("pad_dpad_pressed_v");

            keys[PAD_DPAD_UP].w = keys[PAD_DPAD_DOWN].w = keys[PAD_DPAD_LEFT].w = keys[PAD_DPAD_RIGHT].w =
                keys[PAD_DPAD_UP].h = keys[PAD_DPAD_DOWN].h = keys[PAD_DPAD_LEFT].h = keys[PAD_DPAD_RIGHT].h =
                    (uint16_t) cfg->get_int("pad_dpad_dim");

            keys[PAD_DPAD_UP].column = (uint16_t) cfg->get_int("pad_dpad_up_x");
            keys[PAD_DPAD_UP].row = (uint16_t) cfg->get_int("pad_dpad_up_y");
            keys[PAD_DPAD_DOWN].column = (uint16_t) cfg->get_int("pad_dpad_down_x");
            keys[PAD_DPAD_DOWN].row = (uint16_t) cfg->get_int("pad_dpad_down_y");
            keys[PAD_DPAD_LEFT].column = (uint16_t) cfg->get_int("pad_dpad_left_x");
            keys[PAD_DPAD_LEFT].row = (uint16_t) cfg->get_int("pad_dpad_left_y");
            keys[PAD_DPAD_RIGHT].column = (uint16_t) cfg->get_int("pad_dpad_right_x");
            keys[PAD_DPAD_RIGHT].row = (uint16_t) cfg->get_int("pad_dpad_right_y");

            for (auto & key : keys)
            {
                m_layout.m_keys.emplace_back(key);
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

    if (cfg->has_errors())
    {
        blog(LOG_WARNING, "[ccl] %s", cfg->get_error_message().c_str());
        if (cfg->has_fatal_errors())
        {
            m_layout.m_is_loaded = false;
        }
    }

#ifndef _DEBUG
    if (cfg->get_bool("debug"))
#endif
    {
	    blog(LOG_INFO, "------ input-overlay DEBUG");
	    cfg->dump_structure();
	    for (int i = 0; i < m_layout.m_key_count; i++)
	    {
		    blog(LOG_INFO, "KEY ID: %02d, CODE: %#04X, X: %05d, Y: %05d, U: %05d, V: %05d, W: %05d, H: %05d, X_OFF: %05d",
			    i, m_layout.m_keys[i].m_key_code,
			    m_layout.m_keys[i].column, m_layout.m_keys[i].row,
			    m_layout.m_keys[i].texture_u, m_layout.m_keys[i].texture_v,
			    m_layout.m_keys[i].w, m_layout.m_keys[i].h,
			    m_layout.m_keys[i].x_offset);
	    }
	    blog(LOG_INFO, "------");
    }

    delete cfg;
}

void InputSource::unload_layout()
{
    if (!m_layout.m_keys.empty())
    {
        m_layout.m_keys.clear();
    }

    if (m_gamepad)
        m_gamepad->unload();
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
            if (m_gamepad)
                m_gamepad->check_keys();
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

bool is_mouse_changed(obs_properties_t * props, obs_property_t * p, obs_data_t * s)
{
    bool is_mouse = obs_data_get_bool(s, S_IS_MOUSE);

    obs_property_t * sens = obs_properties_get(props, S_MOUSE_SENS);
    obs_property_t * use_center = obs_properties_get(props, S_MONITOR_USE_CENTER);
    obs_property_t * mon_w = obs_properties_get(props, S_MONITOR_H_CENTER);
    obs_property_t * mon_h = obs_properties_get(props, S_MONITOR_V_CENTER);
    obs_property_t * dead_zone = obs_properties_get(props, S_MOUSE_DEAD_ZONE);

    obs_property_set_visible(sens, is_mouse);
    obs_property_set_visible(use_center, is_mouse);
    obs_property_set_visible(mon_w, is_mouse);
    obs_property_set_visible(mon_h, is_mouse);
    obs_property_set_visible(dead_zone, is_mouse);
    
    return true;
}

bool use_monitor_center_changed(obs_properties_t * props, obs_property_t * p, obs_data_t * s)
{
    bool use_center = obs_data_get_bool(s, S_MONITOR_USE_CENTER);
    obs_property_set_visible(GET_PROPS(S_MONITOR_H_CENTER), use_center);
    obs_property_set_visible(GET_PROPS(S_MONITOR_V_CENTER), use_center);

    return true;
}

obs_properties_t * get_properties_for_overlay(void * data)
{
    InputSource * s = reinterpret_cast<InputSource*>(data);

    obs_properties_t *props = obs_properties_create();

    std::string img_path;
    std::string layout_path;
    std::string filter_img = util_file_filter(T_FILTER_IMAGE_FILES, "*.jpg *.png *.bmp");
    std::string filter_text = util_file_filter(T_FILTER_TEXT_FILES, "*.ini");

    if (s)
    {
        if (!s->m_image_file.empty())
        {
            img_path = s->m_image_file;
            util_format_path(img_path);
        }
        if (!s->m_layout_file.empty())
        {
            layout_path = s->m_layout_file;
            util_format_path(layout_path);
        }
    }

    obs_properties_add_path(props, S_OVERLAY_FILE, T_OVERLAY_FILE, OBS_PATH_FILE,
        filter_img.c_str(), img_path.c_str());

    obs_properties_add_path(props, S_LAYOUT_FILE, T_LAYOUT_FILE, OBS_PATH_FILE,
        filter_text.c_str(), layout_path.c_str());

    // Mouse stuff
    obs_property_t * is_Mouse = obs_properties_add_bool(props, S_IS_MOUSE, T_IS_MOUSE);
    obs_property_set_modified_callback(is_Mouse, is_mouse_changed);

    obs_properties_add_int_slider(props, S_MOUSE_SENS, T_MOUSE_SENS, 1, 500, 1);

    obs_property_t *use_center = obs_properties_add_bool(props, S_MONITOR_USE_CENTER, T_MONITOR_USE_CENTER);
    obs_property_set_modified_callback(use_center, use_monitor_center_changed);

    obs_properties_add_int(props, S_MONITOR_H_CENTER, T_MONITOR_H_CENTER, -9999, 9999, 1);
    obs_properties_add_int(props, S_MONITOR_V_CENTER, T_MONITOR_V_CENTER, -9999, 9999, 1);
    obs_properties_add_int_slider(props, S_MOUSE_DEAD_ZONE, T_MOUSE_DEAD_ZONE, 0, 50, 1);

    // Gamepad stuff

    obs_property_t *is_controller = obs_properties_add_bool(props, S_IS_CONTROLLER, T_IS_CONTROLLER);
    obs_property_set_modified_callback(is_controller, is_controller_changed);
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
    si.create = [](obs_data_t *settings, obs_source_t *source) 
        { return (void*) new InputSource(source, settings);    };
    si.destroy = [](void *data) { delete reinterpret_cast<InputSource*>(data);    };
    si.get_width = [](void *data) { return reinterpret_cast<InputSource*>(data)->cx;    };
    si.get_height = [](void *data) { return reinterpret_cast<InputSource*>(data)->cy;    };

    si.get_defaults = [](obs_data_t *settings)
    {
        // NO-OP
    };

    si.update = [](void *data, obs_data_t *settings)
        { reinterpret_cast<InputSource*>(data)->Update(settings); };
    si.video_tick = [](void *data, float seconds)
        { reinterpret_cast<InputSource*>(data)->Tick(seconds); };
    si.video_render = [](void *data, gs_effect_t *effect)
        { reinterpret_cast<InputSource*>(data)->Render(effect); };
    obs_register_source(&si);
}
