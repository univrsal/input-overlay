/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2022 univrsal <uni@vrsal.xyz>.
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
#include "client_util.hpp"
#include <cstdarg>
#include <cstdio>
#include <util.hpp>

namespace uiohook_helper {
uint32_t last_scroll_time;
std::atomic<bool> hook_state;
std::mutex buffer_mutex;
buffer buf;
static void logger_proc(unsigned int level, void *, const char *format, va_list args)

{
    switch (level) {
    default:
#ifdef USE_DEBUG

    case LOG_LEVEL_DEBUG:
    case LOG_LEVEL_INFO:
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
#endif
        break;
    case LOG_LEVEL_WARN:
    case LOG_LEVEL_ERROR:
        vfprintf(stderr, format, args);
        break;
    }
}

static void logger(unsigned int level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_proc(level, nullptr, format, args);
    va_end(args);
}

void dispatch_proc(uiohook_event *const event, void *)
{
    std::lock_guard<std::mutex> lock(buffer_mutex);
    switch (event->type) {
    case EVENT_HOOK_ENABLED:
        DEBUG_LOG("uiohook started");
        hook_state = true;
        break;
    case EVENT_HOOK_DISABLED:
        DEBUG_LOG("uiohook exited");
        break;
    case EVENT_MOUSE_CLICKED:
    case EVENT_MOUSE_PRESSED:
    case EVENT_MOUSE_RELEASED:
        if (util::cfg.monitor_mouse) {
            //            buf.write<uint8_t>(network::MSG_UIOHOOK_EVENT);
            //            buf.write<uiohook_event>(*event);
        }
        break;
    case EVENT_MOUSE_WHEEL:
        if (util::cfg.monitor_mouse) {
            last_scroll_time = util::get_ticks();
            //            buf.write<uint8_t>(network::MSG_UIOHOOK_EVENT);
            //            buf.write<uiohook_event>(*event);
        }
        break;
    case EVENT_MOUSE_MOVED:
    case EVENT_MOUSE_DRAGGED:
        if (util::cfg.monitor_mouse) {
            //            buf.write<uint8_t>(network::MSG_UIOHOOK_EVENT);
            //            buf.write<uiohook_event>(*event);
        }
        break;
    case EVENT_KEY_TYPED:
    case EVENT_KEY_PRESSED:
    case EVENT_KEY_RELEASED:
        if (util::cfg.monitor_keyboard) {
            //            buf.write<uint8_t>(network::MSG_UIOHOOK_EVENT);
            //            buf.write<uiohook_event>(*event);
        }
        break;
    default:;
    }
}

bool start()
{
    hook_set_logger_proc(&logger_proc, nullptr);
    hook_set_dispatch_proc(&dispatch_proc, nullptr);

    const auto status = hook_run();

    switch (status) {
    case UIOHOOK_SUCCESS:
        hook_state = true;
        return true;
    case UIOHOOK_ERROR_OUT_OF_MEMORY:
        logger(LOG_LEVEL_ERROR, "[uiohook] Failed to allocate memory. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_OPEN_DISPLAY:
        logger(LOG_LEVEL_ERROR, "[uiohook] Failed to open X11 display. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_NOT_FOUND:
        logger(LOG_LEVEL_ERROR, "[uiohook] Unable to locate XRecord extension. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_ALLOC_RANGE:
        logger(LOG_LEVEL_ERROR, "[uiohook] Unable to allocate XRecord range. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_CREATE_CONTEXT:
        logger(LOG_LEVEL_ERROR, "[uiohook] Unable to allocate XRecord context. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_ENABLE_CONTEXT:
        logger(LOG_LEVEL_ERROR, "[uiohook] Failed to enable XRecord context. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_SET_WINDOWS_HOOK_EX:
        logger(LOG_LEVEL_ERROR, "[uiohook] Failed to register low level windows hook. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_AXAPI_DISABLED:
        logger(LOG_LEVEL_ERROR, "[uiohook] Failed to enable access for assistive devices. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_EVENT_PORT:
        logger(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple event port. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_RUN_LOOP_SOURCE:
        logger(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple run loop source. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_GET_RUNLOOP:
        logger(LOG_LEVEL_ERROR, "[uiohook] Failed to acquire apple run loop. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_OBSERVER:
        logger(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple run loop observer. (%#X)", status);
        return false;
    case UIOHOOK_FAILURE:
        logger(LOG_LEVEL_ERROR, "[uiohook] An unknown hook error occurred. (%#X)", status);
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

    DEBUG_LOG("Closing hook");
    switch (status) {
    case UIOHOOK_ERROR_OUT_OF_MEMORY:
        logger(LOG_LEVEL_ERROR, "[uiohook] Failed to allocate memory. (%#X)", status);
        break;
    case UIOHOOK_ERROR_X_RECORD_GET_CONTEXT:
        logger(LOG_LEVEL_ERROR, "[uiohook] Failed to get XRecord context. (%#X)", status);
        break;
    case UIOHOOK_FAILURE:
        logger(LOG_LEVEL_ERROR, "[uiohook] An unknown hook error occurred. (%#X)", status);
        break;
    default:;
    }
}
}
