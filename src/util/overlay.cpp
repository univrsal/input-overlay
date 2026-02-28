/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.cc/alex/input-overlay
 * Copyright 2025 univrsal <uni@vrsal.xyz>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "overlay.hpp"
#include "../sources/input_source.hpp"
#include "config.hpp"
#include "element/element.hpp"
#include "element/element_analog_stick.hpp"
#include "element/element_button.hpp"
#include "element/element_dpad.hpp"
#include "element/element_gamepad_id.hpp"
#include "element/element_mouse_movement.hpp"
#include "element/element_mouse_wheel.hpp"
#include "element/element_trigger.hpp"
#include "../hook/gamepad_hook_helper.hpp"
#include "../hook/uiohook_helper.hpp"
#include "log.h"
#include "../network/remote_connection.hpp"
#include "obs_util.hpp"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <layout_constants.h>

namespace sources {
class overlay_settings;
}

static const std::unordered_map<uint16_t, uint16_t> keyCodeMap = {
    {0x0001, VC_ESCAPE},
    {0x003B, VC_F1},
    {0x003C, VC_F2},
    {0x003D, VC_F3},
    {0x003E, VC_F4},
    {0x003F, VC_F5},
    {0x0040, VC_F6},
    {0x0041, VC_F7},
    {0x0042, VC_F8},
    {0x0043, VC_F9},
    {0x0044, VC_F10},
    {0x0057, VC_F11},
    {0x0058, VC_F12},
    {0x005B, VC_F13},
    {0x005C, VC_F14},
    {0x005D, VC_F15},
    {0x0063, VC_F16},
    {0x0064, VC_F17},
    {0x0065, VC_F18},
    {0x0066, VC_F19},
    {0x0067, VC_F20},
    {0x0068, VC_F21},
    {0x0069, VC_F22},
    {0x006A, VC_F23},
    {0x006B, VC_F24},
    {0x0002, VC_1},
    {0x0003, VC_2},
    {0x0004, VC_3},
    {0x0005, VC_4},
    {0x0006, VC_5},
    {0x0007, VC_6},
    {0x0008, VC_7},
    {0x0009, VC_8},
    {0x000A, VC_9},
    {0x000B, VC_0},
    {0x000C, VC_MINUS},
    {0x000D, VC_EQUALS},
    {0x000E, VC_BACKSPACE},
    {0x000F, VC_TAB},
    {0x003A, VC_CAPS_LOCK},
    {0x001E, VC_A},
    {0x0030, VC_B},
    {0x002E, VC_C},
    {0x0020, VC_D},
    {0x0012, VC_E},
    {0x0021, VC_F},
    {0x0022, VC_G},
    {0x0023, VC_H},
    {0x0017, VC_I},
    {0x0024, VC_J},
    {0x0025, VC_K},
    {0x0026, VC_L},
    {0x0032, VC_M},
    {0x0031, VC_N},
    {0x0018, VC_O},
    {0x0019, VC_P},
    {0x0010, VC_Q},
    {0x0013, VC_R},
    {0x001F, VC_S},
    {0x0014, VC_T},
    {0x0016, VC_U},
    {0x002F, VC_V},
    {0x0011, VC_W},
    {0x002D, VC_X},
    {0x0015, VC_Y},
    {0x002C, VC_Z},
    {0x001A, VC_OPEN_BRACKET},
    {0x001B, VC_CLOSE_BRACKET},
    {0x002B, VC_BACK_SLASH},
    {0x0027, VC_SEMICOLON},
    {0x0028, VC_QUOTE},
    {0x001C, VC_ENTER},
    {0x0033, VC_COMMA},
    {0x0034, VC_PERIOD},
    {0x0035, VC_SLASH},
    {0x0039, VC_SPACE},
    {0x0046, VC_SCROLL_LOCK},
    {0x0E45, VC_PAUSE},
    {0x0E52, VC_INSERT},
    {0x0E53, VC_DELETE},
    {0x0E47, VC_HOME},
    {0x0E4F, VC_END},
    {0x0E49, VC_PAGE_UP},
    {0x0E51, VC_PAGE_DOWN},
    {0xE048, VC_UP},
    {0xE04B, VC_LEFT},
    {0xE04D, VC_RIGHT},
    {0xE050, VC_DOWN},
    {0x0045, VC_NUM_LOCK},
    {0x0E35, VC_KP_DIVIDE},
    {0x0037, VC_KP_MULTIPLY},
    {0x004A, VC_KP_SUBTRACT},
    {0x0E0D, VC_KP_EQUALS},
    {0x004E, VC_KP_ADD},
    {0x0E1C, VC_KP_ENTER},
    {0x0053, VC_KP_SEPARATOR},
    {0x004F, VC_KP_1},
    {0x0050, VC_KP_2},
    {0x0051, VC_KP_3},
    {0x004B, VC_KP_4},
    {0x004C, VC_KP_5},
    {0x004D, VC_KP_6},
    {0x0047, VC_KP_7},
    {0x0048, VC_KP_8},
    {0x0049, VC_KP_9},
    {0x0052, VC_KP_0},
    {0xEE00, VC_KP_CLEAR},
    {0x002A, VC_SHIFT_L},
    {0x0036, VC_SHIFT_R},
    {0x001D, VC_CONTROL_L},
    {0x0E1D, VC_CONTROL_R},
    {0x0038, VC_ALT_L},
    {0x0E38, VC_ALT_R},
    {0x0E5B, VC_META_L},
    {0x0E5C, VC_META_R},
    {0x0E5D, VC_CONTEXT_MENU},
    {0xE05E, VC_POWER},
    {0xE05F, VC_SLEEP},
    {0xE063, VC_WAKE},
    {0xE022, VC_MEDIA_PLAY},
    {0xE024, VC_MEDIA_STOP},
    {0xE010, VC_MEDIA_PREVIOUS},
    {0xE019, VC_MEDIA_NEXT},
    {0xE06D, VC_MEDIA_SELECT},
    {0xE02C, VC_MEDIA_EJECT},
    {0xE020, VC_VOLUME_MUTE},
    {0xE030, VC_VOLUME_UP},
    {0xE02E, VC_VOLUME_DOWN},
    {0xE06C, VC_APP_MAIL},
    {0xE021, VC_APP_CALCULATOR},
    {0xE065, VC_BROWSER_SEARCH},
    {0xE032, VC_BROWSER_HOME},
    {0xE06A, VC_BROWSER_BACK},
    {0xE069, VC_BROWSER_FORWARD},
    {0xE068, VC_BROWSER_STOP},
    {0xE067, VC_BROWSER_REFRESH},
    {0xE066, VC_BROWSER_FAVORITES},
    {0x0070, VC_KATAKANA},
    {0x0073, VC_UNDERSCORE},
    {0x0079, VC_KANJI},
    {0x007B, VC_HIRAGANA},
    {0x007D, VC_YEN},
    {0x0000, VC_UNDEFINED},
    {0xFFFF, CHAR_UNDEFINED},
};

overlay::~overlay()
{
    unload();
}

overlay::overlay(sources::overlay_settings *settings)
{
    m_settings = settings;
    m_is_loaded = load();
}

bool overlay::load()
{
    // Unload attempts to access uninitialized member from struct gs_image_file4 when unload is called
    if (m_image)
        unload();
    const auto image_loaded = load_texture();
    m_is_loaded = image_loaded && load_cfg();

    if (!m_is_loaded) {
        //        m_settings->gamepad = 0;
        if (!image_loaded) {
            m_settings->cx = 100; /* Default size */
            m_settings->cy = 100;
        }
    }

    return m_is_loaded;
}

void overlay::unload()
{
    unload_texture();
    unload_elements();
    m_settings->cx = 100;
    m_settings->cy = 100;
}

bool overlay::load_cfg()
{
    if (!m_settings || m_settings->layout_file.empty())
        return false;

    QFile file(m_settings->layout_file.c_str());

    if (!file.open(QIODevice::ReadOnly)) {
        blog(LOG_ERROR, "[input-overlay] couldn't open config file");
        return false;
    }

    QJsonParseError err;
    const auto cfg_doc = QJsonDocument::fromJson(file.readAll(), &err);
    auto cfg_obj = cfg_doc.object();
    const auto flag = true;

    if (err.error == QJsonParseError::NoError) {
        bool needs_conversion = false;
        if (!cfg_obj[CFG_VERSION].isDouble() || cfg_obj[CFG_VERSION].toDouble() <= CFG_CURRENT_VERSION) {
            bwarn(
                "Loaded config file was created with an older version of input-overlay, trying to convert the key codes to the new format");
            needs_conversion = true;
        }
        m_settings->cx = static_cast<uint32_t>(cfg_obj[CFG_TOTAL_WIDTH].toInt());
        m_settings->cy = static_cast<uint32_t>(cfg_obj[CFG_TOTAL_HEIGHT].toInt());
        m_settings->layout_flags = static_cast<uint8_t>(cfg_obj[CFG_FLAGS].toInt());

        const auto debug_mode = cfg_obj[CFG_DEBUG_FLAG].toBool();

#ifndef _DEBUG
        if (debug_mode) {
#else
        {
#endif
            binfo("Started loading of %s", m_settings->layout_file.c_str());
        }

        auto arr = cfg_obj[CFG_ELEMENTS].toArray();

        for (const auto element : arr)
            load_element(element.toObject(), debug_mode, needs_conversion);
    } else {
        berr("Couldn't load layout from %s. Error: %s", m_settings->layout_file.c_str(), qt_to_utf8(err.errorString()));
    }

    return flag;
}

bool overlay::load_texture()
{
    if (!m_settings || m_settings->image_file.empty())
        return false;

    auto flag = true;
    unload_texture();
    if (m_image == nullptr) {
        m_image = new gs_image_file4_t();
    }

    gs_image_file4_init(m_image, m_settings->image_file.c_str(),
                        m_settings->linear_alpha ? GS_IMAGE_ALPHA_PREMULTIPLY_SRGB : GS_IMAGE_ALPHA_PREMULTIPLY);

    obs_enter_graphics();
    gs_image_file4_init_texture(m_image);
    obs_leave_graphics();

    if (!get_texture()->loaded) {
        bwarn("Error: failed to load texture %s", m_settings->image_file.c_str());
        flag = false;
        /*If image is loaded, but m_is_loaded is false, then config file is not loaded
    Conversely, if m_is_loaded returns true then linear_alpha_changed called load_texture()*/
    } else if (!m_is_loaded) {
        m_settings->cx = get_texture()->cx;
        m_settings->cy = get_texture()->cy;
    }

    return flag;
}

void overlay::unload_texture()
{
    if (m_image) {
        obs_enter_graphics();
        gs_image_file4_free(m_image);
        obs_leave_graphics();
        delete m_image;
        m_image = nullptr;
    }
}

void overlay::unload_elements()
{
    m_elements.clear();
}

void overlay::draw(gs_effect_t *effect)
{
    if (m_is_loaded) {
        for (auto const &element : m_elements) {
            element->draw(effect, get_texture(), m_settings);
        }
    }
}

void overlay::tick(float seconds)
{
    if (m_is_loaded) {
        for (auto const &element : m_elements) {
            element->tick(seconds, m_settings);
        }
    }
}

void overlay::refresh_data()
{
    if (io_config::io_window_filters.input_blocked())
        return;
    if (!(uiohook::state || wss::state || gamepad_hook::state))
        return;

    /* This copies over necessary input data information
     * to make sure the overlay always has data available to
     * draw the overlay. If the data was directly accessed in the render
     * method, the overlay can start to flicker if the frame is rendered
     * while the data is currently inaccessible, because it is being written
     * to by the input thread, resulting in all buttons being unpressed
     */

    if (m_settings->use_local_input()) {

        local_data::data.m_mutex.lock();
        m_settings->data.copy(&local_data::data);
        if (uiohook::state)
            uiohook::check_wheel();

        if (m_settings->gamepad) {
            m_settings->gamepad->mutex().lock();
            m_settings->gamepad->copy_data(&m_settings->data, m_settings->gamepad_name);
            m_settings->gamepad->mutex().unlock();
        }
        local_data::data.m_mutex.unlock();
    } else if (wss::state) {
        if (m_settings->remote_input_data) {
            m_settings->remote_input_data->m_mutex.lock();
            m_settings->data.copy(m_settings->remote_input_data.get(), true);
            m_settings->remote_input_data->m_mutex.unlock();
        }
    }
}

void overlay::load_element(const QJsonObject &obj, bool debug, bool needs_conversion)
{
    const auto type = obj[CFG_TYPE].toInt();
    element *new_element = nullptr;

    switch (type) {
    case ET_TEXTURE:
        new_element = new element_texture;
        break;
    case ET_GAMEPAD_ID:
        new_element = new element_gamepad_id;
        break;
    case ET_KEYBOARD_KEY:
        new_element = new element_keyboard_key;
        break;
    case ET_MOUSE_BUTTON:
        new_element = new element_mouse_button;
        break;
    case ET_GAMEPAD_BUTTON:
        new_element = new element_gamepad_button;
        break;
    case ET_WHEEL:
        new_element = new element_wheel;
        break;
    case ET_TRIGGER:
        new_element = new element_trigger;
        break;
    case ET_ANALOG_STICK:
        new_element = new element_analog_stick;
        break;
    case ET_DPAD_STICK:
        new_element = new element_dpad;
        break;
    case ET_MOUSE_MOVEMENT:
        new_element = new element_mouse_movement;
        break;
    default:
        if (debug)
            binfo("Invalid element type %i for %s", type, qt_to_utf8(obj[CFG_ID].toString()));
    }

    if (new_element) {
        new_element->load(obj);
        if (new_element->get_type() == ET_KEYBOARD_KEY && needs_conversion) {
            auto *key_element = static_cast<element_keyboard_key *>(new_element);
            const auto keycode = key_element->get_keycode();
            const auto it = keyCodeMap.find(keycode);
            if (it != keyCodeMap.end()) {
                key_element->set_keycode(it->second);
            } else {
                bwarn("Failed to convert keycode 0x%04X for element '%s'", keycode, qt_to_utf8(obj[CFG_ID].toString()));
            }
        }
        m_elements.emplace_back(new_element);

#ifndef _DEBUG
        if (debug) {
#else
        {
#endif
            binfo("Type: %14s, KEYCODE: 0x%04X ID: %s", element_type_to_string(static_cast<element_type>(type)),
                  new_element->get_keycode(), qt_to_utf8(obj[CFG_ID].toString()));
        }
    }
}

const char *overlay::element_type_to_string(const element_type t)
{
    switch (t) {
    case ET_TEXTURE:
        return "Texture";
    case ET_KEYBOARD_KEY:
        return "Keyboard key";
    case ET_MOUSE_BUTTON:
        return "Mouse button";
    case ET_GAMEPAD_BUTTON:
        return "Gamepad button";
    case ET_ANALOG_STICK:
        return "Analog stick";
    case ET_WHEEL:
        return "Scroll wheel";
    case ET_MOUSE_MOVEMENT:
        return "Mouse movement";
    case ET_TRIGGER:
        return "Trigger";
    case ET_GAMEPAD_ID:
        return "Gamepad ID";
    case ET_DPAD_STICK:
        return "DPad";
    default:
    case ET_INVALID:
        return "Invalid";
    }
}
