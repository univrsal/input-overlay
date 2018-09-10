/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "Dialog.hpp"
#include "../util/SDL_Helper.hpp"
#include "elements/Textbox.hpp"
#include "elements/Button.hpp"
#include "elements/Combobox.hpp"
#include "../../../ccl/ccl.hpp"

class Tool;

class SDL_Helper;

class Notifier;

class DialogSetup : public Dialog
{
public:
    DialogSetup(SDL_Helper * sdl, Notifier * notifier, Tool * t)
        : Dialog(sdl, SDL_Point { 500, 280 }, LANG_DIALOG_SETUP)
    {
        m_notifier = notifier;
        m_tool = t;
    };

    void init() override;

    void draw_background(void) override;
    
    void action_performed(int8_t action_id) override;

    SDL_Point get_default_dim() const;

    bool should_load_cfg(void) const;

    const char * get_config_path(void) const;

    const char * get_texture_path(void) const;
private:
    Notifier * m_notifier = nullptr;

    bool m_load_cfg = false;

    Combobox * m_lang_box = nullptr;
    Textbox * m_texture_path = nullptr;
    Textbox * m_config_path = nullptr;
    Textbox * m_def_w = nullptr;
    Textbox * m_def_h = nullptr;
    Textbox * m_h_space = nullptr;
    Textbox * m_v_space = nullptr;

    Tool * m_tool = nullptr;
public:
    SDL_Point get_rulers() const;
};
