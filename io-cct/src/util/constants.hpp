/**
 * Created by universal on 21.07.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

/* sdl_helper constants */
#define SDL_WINDOW_W                    1280
#define SDL_WINDOW_H                    720
#define SDL_WINDOW_FPS                  30
#define SDL_WINDOW_TPF                  (1000 / SDL_WINDOW_FPS)
#define SDL_WINDOW_TITLE                "io-cct"

/* 0/0 for coordinate systems */
#define X_AXIS 110
#define Y_AXIS 110

/* Resources */
#define PATH_UNICODE_FONT               "res/unifont.ttf"
#define PATH_TRANSLATIONS               "res/lang"
#define PATH_WINDOW_ICON                "res/icon.png"

#define ERROR_NO_TRANSLATION            "Error: No translation was found. Make sure that at least en_US.ini is in ./res/lang/"

#define LABEL_BUILD                     "io-cct build "
#define ELEMENT_UNNAMED                 "Unnamed"
#define LABEL_X_AXIS                    "X in pixels"
#define LABEL_Y_AXIS                    "Y in pixels"
#define LABEL_SCALE                     "Scale: %ix"

/* Translation constants*/
#define LANG_ID                         "language"

/* Coordinate system */
#define LANG_LABEL_X_AXIS               "label_x_axis"
#define LANG_LABEL_Y_AXIS               "label_y_axis"
#define LANG_LABEL_SCALE                "label_scale"

/* SDL errors */
#define SDL_INIT_FAILED                 "Initialization of SDL failed! SDL_Error: %s\n"
#define SDL_CREATE_WINDOW_FAILED        "Creating SDL Window failed! SDL_Error: %s\n"
#define SDL_LOAD_WINDOW_ICON_FAILED     "Loading window icon failed! SDL_Error: %s\n"
#define SDL_CREATE_RENDERER_FAILED      "Creating SDL Renderer failed! SDL_Error: %s\n"
#define SDL_TTF_INIT_FAILED             "Initializing SDL_ttf failed! SDL_Error: %s\n"
#define SDL_FONT_LOADING_FAILED         "Couldn't load fonts!\n"
#define SDL_IMAGE_TO_SURFACE            "Error: Couldn't create surface %s! SDL_Error: %s\n"
#define SDL_SURFACE_TO_TEXTURE_FAILED   "Error: Couldn't load image %s! SDL_Error: %s\n"
#define SDL_TEXT_TO_TEXTURE             "Error while creating texture from surface in text rendering! SDL_Error: %s\n"
#define SDL_TEXT_TO_SURFACE             "Error while creating surface from font in text rendering! SDL_Error: %s\n"
#define SDL_OPEN_CONTROLLER_FAILED      "Error while opening controller with id %i! SDL_Error: %s\n"

/* Notifications */
#define LANG_MSG_SAVE_ERROR             "msg_save_error"
#define LANG_MSG_LOAD_ERROR             "msg_load_error"
#define LANG_MSG_SAVE_SUCCESS           "msg_save_success"
#define LANG_MSG_LOAD_SUCCESS           "msg_load_success"
#define LANG_MSG_CONFIG_EMPTY           "msg_config_empty"
#define LANG_MSG_CONFIG_CORRUPT         "msg_config_corrupt"
#define LANG_MSG_VALUE_TYPE_INVALID     "msg_value_invalid_type"
#define LANG_MSG_NOTHING_TO_SAVE        "msg_nothing_to_save"
#define LANG_MSG_GAMEPAD_CONNECTED      "msg_gamepad_connected"
#define LANG_MSG_GAMEPAD_DISCONNECTED   "msg_gamepad_disconnected"
#define LANG_MSG_ELEMENT_LOAD_ERROR     "msg_element_load_error"

/* Dialog titles*/
#define LANG_DIALOG_NEW_ELEMENT         "dialog_new_element"
#define LANG_DIALOG_SETUP               "dialog_setup"
#define LANG_DIALOG_HELP                "dialog_help"
#define LANG_DIALOG_ELEMENT_TYPE        "dialog_element_type"
#define LANG_DIALOG_ELEMENT_SETTINGS    "dialog_element_settings"

/* Errors */
#define LANG_ERROR_ID_NOT_UNIQUE        "error_id_not_unique"
#define LANG_ERROR_KEYCODE_INVALID      "error_keycode_invalid"
#define LANG_ERROR_RADIUS_INVALID       "error_radius_invalid"
#define LANG_ERROR_SELECTION_EMTPY      "error_selection_empty"
#define LANG_ERROR_INVALID_TEXTURE_PATH "error_invalid_texture_path"
#define LANG_ERROR_INVALID_CONFIG_PATH  "error_invalid_config_path"
#define LANG_ERROR_ID_EMPTY             "error_id_empty"
#define LANG_ERROR_TYPE_INVALID         "error_type_invalid"

/* Setup dialog */
#define LANG_LABEL_INFO                 "label_info"
#define LANG_LABEL_TEXTURE_PATH         "label_texture_path"
#define LANG_LABEL_CONFIG_PATH          "label_config_path"
#define LANG_LABEL_DEFAULT_WIDTH        "label_default_width"
#define LANG_LABEL_DEFAULT_HEIGHT       "label_default_height"
#define LANG_LABEL_ELEMENT_H_SPACE      "label_element_h_space"
#define LANG_LABEL_ELEMENT_V_SPACE      "label_element_v_space"

/* Reusable elements */
#define LANG_BUTTON_OK                  "button_ok"
#define LANG_BUTTON_EXIT                "button_exit"
#define LANG_BUTTON_CANCEL              "button_cancel"

#define LANG_LABEL_WIDTH                "label_width"
#define LANG_LABEL_HEIGHT               "label_height"
#define LANG_LABEL_X                    "label_x"
#define LANG_LABEL_Y                    "label_y"
#define LANG_LABEL_Z_LEVEL              "label_z_level"
#define LANG_LABEL_U                    "label_u"
#define LANG_LABEL_V                    "label_v"
#define LANG_LEFT                       "left"
#define LANG_RIGHT                      "right"
#define LANG_UP                         "up"
#define LANG_DOWN                       "down"

/* New Element Dialog */
#define LANG_LABEL_TEXTURE_SELECTION    "label_texture_selection"
#define LANG_LABEL_KEY_CODE             "label_keycode"
#define LANG_LABEL_ELEMENT_ID           "label_element_id"
#define LANG_CHECKBOX_RECORD_KEYBIND    "checkbox_record_keybind"
#define LANG_LABEL_STICK_SIDE           "label_stick_side"
#define LANG_LABEL_MOUSE_TYPE           "label_mouse_type"
#define LANG_LABEL_WHEEL_INFO           "label_wheel_info"
#define LANG_LABEL_MOVEMENT_RADIUS      "label_movement_radius"
#define LANG_ITEM_MOUSE_TYPE_ARROW      "item_mouse_type_arrow"
#define LANG_ITEM_MOUSE_TYPE_DOT        "item_mouse_type_dot"
#define LANG_LABEL_TRIGGER_DIRECTION    "label_trigger_direction"
#define LANG_LABEL_TRIGGER_SIDE         "label_trigger_side"
#define LANG_CHECKBOX_TRIGGER_BUTON     "checkbox_trigger_button"
#define LANG_LABEL_GAMEPAD_ID_INFO      "label_gamepad_id_info"
#define LANG_LABEL_DPAD_INFO            "label_dpad_info"

/* Selected element settings */
#define LANG_BUTTON_ADD_ELEMENT         "button_add_element"
#define LANG_BUTTON_DELETE_ELEMENT      "button_delete_element"
#define LANG_BUTTON_MODIFY_ELEMENT      "button_modify_element"
#define LANG_BUTTON_SAVE_CONFIG         "button_save_config"
#define LANG_BUTTON_HELP                "button_help"

/* Help and about dialog*/
#define LANG_LABEL_HELP_AND_ABOUT       "label_help_about"
#define LANG_BUTTON_OPEN_URL            "button_open_url"

/* Dialog element type */
#define LANG_LABEL_SELECT_ELEMENT_TYPE  "label_element_type"
#define LANG_ELEMENT_TEXTURE            "element_texture"
#define LANG_ELEMENT_BUTTON             "element_button"
#define LANG_ELEMENT_MOUSE_WHEEL        "element_mouse_wheel"
#define LANG_ELEMENT_MOUSE_MOVEMENT     "element_mouse_movement"
#define LANG_ELEMENT_ANALOG_STICK       "element_analog_stick"
#define LANG_ELEMENT_TRIGGER            "element_trigger"
#define LANG_ELEMENT_DPAD_STICK         "element_dpad_stick"
#define LANG_ELEMENT_GAMEPAD_ID         "element_gamepad_id"
