/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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

#include "uiohook_helper.hpp"
#include <cstdarg>
#include <cstdio>
#include <util.hpp>
#include "../util/log.h"

namespace uiohook {
std::mutex hook_mutex;
uint32_t last_scroll_time;
volatile bool hook_state = false;

bool logger_proc(unsigned level, const char *format, ...)
{
    va_list args;
    std::string f;
    switch (level) {
    case LOG_LEVEL_WARN:
    case LOG_LEVEL_ERROR:
        va_start(args, format);
        f = std::string(format);
        f.insert(0, "[input-overlay] ");
        blog(LOG_WARNING, f.c_str(), args);
        va_end(args);
    default:;
    }
    return true;
}

void dispatch_proc(uiohook_event *const event)
{
    //if (!input_data)
    //return;
    std::lock_guard<std::mutex> lock(hook_mutex);
    //check_wheel();

    switch (event->type) {
    case EVENT_KEY_PRESSED:
    case EVENT_KEY_RELEASED: /* Fallthrough */
        //input_data->add_data(event->data.keyboard.keycode,
        //					 new element_data_button(event->type == EVENT_KEY_PRESSED ? BS_PRESSED : BS_RELEASED));
        break;
    case EVENT_MOUSE_WHEEL:
        //last_wheel = os_gettime_ns();
        //if (event->data.wheel.rotation >= WHEEL_DOWN) {
        //	input_data->add_data(VC_MOUSE_WHEEL_DOWN, new element_data_button(BS_PRESSED));
        //	input_data->remove_data(VC_MOUSE_WHEEL_UP);
        //} else {
        //	input_data->add_data(VC_MOUSE_WHEEL_UP, new element_data_button(BS_PRESSED));
        //	input_data->remove_data(VC_MOUSE_WHEEL_DOWN);
        //}
        break;
    case EVENT_MOUSE_PRESSED:
    case EVENT_MOUSE_RELEASED:
        //input_data->add_data(common::util_mouse_to_vc(event->data.mouse.button),
        //					 new element_data_button(event->type == EVENT_MOUSE_PRESSED ? BS_PRESSED : BS_RELEASED));

        //last_character = event->data.keyboard.keychar;
        break;
    case EVENT_MOUSE_DRAGGED:
    case EVENT_MOUSE_MOVED:
        //input_data->add_data(VC_MOUSE_DATA, new element_data_mouse_pos(event->data.mouse.x, event->data.mouse.y));
        break;
    default:;
    }
}

bool start()
{
    hook_set_logger_proc(&logger_proc);
    hook_set_dispatch_proc(&dispatch_proc);

    const auto status = hook_run();

    switch (status) {
    case UIOHOOK_SUCCESS:
        hook_state = true;
        return true;
    case UIOHOOK_ERROR_OUT_OF_MEMORY:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to allocate memory. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_OPEN_DISPLAY:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to open X11 display. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_NOT_FOUND:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Unable to locate XRecord extension. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_ALLOC_RANGE:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Unable to allocate XRecord range. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_CREATE_CONTEXT:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Unable to allocate XRecord context. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_ENABLE_CONTEXT:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to enable XRecord context. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_SET_WINDOWS_HOOK_EX:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to register low level windows hook. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_AXAPI_DISABLED:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to enable access for assistive devices. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_EVENT_PORT:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple event port. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_RUN_LOOP_SOURCE:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple run loop source. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_GET_RUNLOOP:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to acquire apple run loop. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_OBSERVER:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple run loop observer. (%#X)", status);
        return false;
    case UIOHOOK_FAILURE:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] An unknown hook error occurred. (%#X)", status);
        return false;
    default:;
        return true; /* Unknown error but we can still try */
    }
}

void stop()
{
    if (!hook_state)
        return;
    hook_state = false;
    const auto status = hook_stop();

    bdebug("Closing hook");
    switch (status) {
    case UIOHOOK_ERROR_OUT_OF_MEMORY:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to allocate memory. (%#X)", status);
        break;
    case UIOHOOK_ERROR_X_RECORD_GET_CONTEXT:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to get XRecord context. (%#X)", status);
        break;
    case UIOHOOK_FAILURE:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] An unknown hook error occurred. (%#X)", status);
        break;
    default:;
    }
}
}
