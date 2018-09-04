/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <SDL.h>
#include "../../io-obs/util/layout_constants.hpp"
#include "element/Element.hpp"

class Dialog;
class Notifier;
class SDL_Helper;
class Element;
class Config;
class DialogElementSettings;
class DialogHelp;
class DialogNewElement;
class Texture;

enum ToolState
{
    IN_SETUP,
    IN_BUILD,
    IN_HELP,
    IN_ELEMENT_TYPE,
    IN_NEW_ELEMENT
};

#define TOOL_ACTION_NEW_ELEMENT_ADD 4
#define TOOL_ACTION_MOD_ELEMENT_APPLY 6
#define TOOL_ACTION_SAVE_CONFIG 7
#define TOOL_ACTION_SETUP_EXIT 8

enum DialogID
{
    NONE,
    HELP,
    NEW_ELEMENT,
    MOD_ELEMENT,
    SELECECT_TYPE
};

class Tool
{
public:
    Tool() { m_helper = nullptr; }
    Tool(SDL_Helper * helper);
    ~Tool();

    void program_loop();

    Element * get_selected(void);

    uint16_t get_selected_id(void);

    void action_performed(uint8_t type);

    Texture * get_atlas(void);

    void delete_element(uint16_t id);

    void set_new_element_type(ElementType type) { m_new_element_type = type; }

    void queue_dialog_open(DialogID id);
    void queue_dialog_close(void);

    ElementError verify_element(DialogNewElement * d);
private:
    void add_element(Element * e);

    void close_toplevel(void);
    void handle_input();
    bool m_run_flag = true;

    bool m_queue_close = false; /* True when top level dialog should be closed */
    DialogID m_queued_dialog = DialogID::NONE;

    /* Stores the type, which the user selected for the next element */
    ElementType m_new_element_type = ElementType::INVALID;
    
    SDL_Event m_event;
    SDL_Helper * m_helper;

    Config * m_config = nullptr;

    DialogElementSettings * m_element_settings = nullptr;
    Dialog * m_toplevel = nullptr;
    Notifier * m_notify = nullptr;
    ToolState m_state;
};
