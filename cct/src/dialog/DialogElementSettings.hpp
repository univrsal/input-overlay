/**
 * Created by univrsal on 27.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "Dialog.hpp"
#include "../util/SDL_Helper.hpp"
#include "../util/Constants.hpp"
#include "./elements/Textbox.hpp"
#include "./elements/Button.hpp"
#include "../Tool.hpp"

#define ACTION_HELP_BUTTON -10
#define ACTION_NEW_ELEMENT -11
#define ACTION_SAVE_CONFIG -12
#define ACTION_DEL_ELEMENT -13
#define ACTION_MOD_ELEMENT -14

class SDL_Helper;

class Tool;

class Element;

class DialogElementSettings : public Dialog
{
public:
    DialogElementSettings(SDL_Helper* sdl, Tool* tool)
        : Dialog(sdl, SDL_Rect{1030, 200, 240, 400}, LANG_DIALOG_ELEMENT_SETTINGS)
    {
        m_tool = tool;
    };

    void init() override;

    void action_performed(int8_t action_id) override;

    void set_wh(int w, int h) const;
    void set_xy(int x, int y) const;
    void set_uv(int u, int v) const;
    void set_id(std::string id) const;
    void set_vc(uint16_t vc) const;
    void set_z_level(uint8_t z) const;
    void select_element(Element* e) const;

    int get_x() const { return SDL_atoi(m_element_x->c_str()); }
    int get_y() const { return SDL_atoi(m_element_y->c_str()); }
    int get_z() const { return SDL_atoi(m_element_z_level->c_str()); }
    int get_u() const { return SDL_atoi(m_element_u->c_str()); }
    int get_v() const { return SDL_atoi(m_element_v->c_str()); }
    int get_w() const { return SDL_atoi(m_element_width->c_str()); }
    int get_h() const { return SDL_atoi(m_element_height->c_str()); }
    uint16_t get_vc() const { return SDL_strtol(m_element_vc->c_str(), nullptr, 16); }
    SDL_Rect get_mapping() const { return {get_u(), get_v(), get_w(), get_h()}; }

    const std::string* get_id() const { return m_element_id->get_text(); }

    void alert_element_id() const { m_element_id->set_alert(true); }
private:
    bool m_finished = false;

    Textbox* m_element_id = nullptr;
    Textbox* m_element_vc = nullptr;
    Textbox* m_element_width = nullptr;
    Textbox* m_element_height = nullptr;
    Textbox* m_element_x = nullptr;
    Textbox* m_element_y = nullptr;
    Textbox* m_element_u = nullptr;
    Textbox* m_element_v = nullptr;
    Textbox* m_element_z_level = nullptr;

    Tool* m_tool = nullptr;
};
