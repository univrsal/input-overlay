/*************************************************************************
 * This file is part of input-overlay
 * github->con/univrsal/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
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
std::atomic<bool> hook_state;
event_queue queue;

static void print_proc(unsigned int level, void *, const char *format, va_list args)
{
    switch (level) {
    default:
    case LOG_LEVEL_DEBUG:
        if (util::cfg.verbose) {
            vfprintf(stdout, format, args) >= 0;
        }
        break;
    case LOG_LEVEL_INFO:
        vfprintf(stdout, format, args) >= 0;
        break;
    case LOG_LEVEL_WARN:
    case LOG_LEVEL_ERROR:
        vfprintf(stderr, format, args) >= 0;
        break;
    }
}

void dispatch_proc(uiohook_event *const event, void *)
{
    switch (event->type) {
    case EVENT_HOOK_ENABLED:
        binfo("uiohook started");
        hook_state = true;
        break;
    case EVENT_HOOK_DISABLED:
        binfo("uiohook exited");
        break;
    case EVENT_MOUSE_CLICKED:
    case EVENT_MOUSE_PRESSED:
    case EVENT_MOUSE_RELEASED:
        if (util::cfg.monitor_mouse) {
            std::lock_guard<std::mutex> lock(queue.mutex);
            queue.events.emplace_back(*event);
        }
        break;
    case EVENT_MOUSE_WHEEL:
        if (util::cfg.monitor_mouse) {
            std::lock_guard<std::mutex> lock(queue.mutex);
            queue.events.emplace_back(*event);
        }
        break;
    case EVENT_MOUSE_MOVED:
    case EVENT_MOUSE_DRAGGED:
        if (util::cfg.monitor_mouse) {
            std::lock_guard<std::mutex> lock(queue.mutex);
            queue.events.emplace_back(*event);
        }
        break;
    case EVENT_KEY_TYPED:
    case EVENT_KEY_PRESSED:
    case EVENT_KEY_RELEASED:
        if (util::cfg.monitor_keyboard) {
            std::lock_guard<std::mutex> lock(queue.mutex);
            queue.events.emplace_back(*event);
        }
        break;
    default:;
    }
}

void print(unsigned int level, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    print_proc(level, nullptr, format, args);
    va_end(args);
}

bool start()
{
    hook_set_logger_proc(&print_proc, nullptr);
    hook_set_dispatch_proc(&dispatch_proc, nullptr);

    const auto status = hook_run();

    switch (status) {
    case UIOHOOK_SUCCESS:
        hook_state = true;
        return true;
    case UIOHOOK_ERROR_OUT_OF_MEMORY:

        print(LOG_LEVEL_ERROR, nullptr, "[uiohook] Failed to allocate memory. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_OPEN_DISPLAY:
        print(LOG_LEVEL_ERROR, nullptr, "[uiohook] Failed to open X11 display. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_NOT_FOUND:
        print(LOG_LEVEL_ERROR, "[uiohook] Unable to locate XRecord extension. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_ALLOC_RANGE:
        print(LOG_LEVEL_ERROR, "[uiohook] Unable to allocate XRecord range. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_CREATE_CONTEXT:
        print(LOG_LEVEL_ERROR, "[uiohook] Unable to allocate XRecord context. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_ENABLE_CONTEXT:
        print(LOG_LEVEL_ERROR, "[uiohook] Failed to enable XRecord context. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_SET_WINDOWS_HOOK_EX:
        print(LOG_LEVEL_ERROR, "[uiohook] Failed to register low level windows hook. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_AXAPI_DISABLED:
        print(LOG_LEVEL_ERROR, "[uiohook] Failed to enable access for assistive devices. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_EVENT_PORT:
        print(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple event port. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_RUN_LOOP_SOURCE:
        print(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple run loop source. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_GET_RUNLOOP:
        print(LOG_LEVEL_ERROR, "[uiohook] Failed to acquire apple run loop. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_OBSERVER:
        print(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple run loop observer. (%#X)", status);
        return false;
    case UIOHOOK_FAILURE:
        print(LOG_LEVEL_ERROR, "[uiohook] An unknown hook error occurred. (%#X)", status);
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

    switch (status) {
    case UIOHOOK_ERROR_OUT_OF_MEMORY:
        print(LOG_LEVEL_ERROR, "[uiohook] Failed to allocate memory. (%#X)", status);
        break;
    case UIOHOOK_ERROR_X_RECORD_GET_CONTEXT:
        print(LOG_LEVEL_ERROR, "[uiohook] Failed to get XRecord context. (%#X)", status);
        break;
    case UIOHOOK_FAILURE:
        print(LOG_LEVEL_ERROR, "[uiohook] An unknown hook error occurred. (%#X)", status);
        break;
    default:;
    }
    binfo("uiohook stopped");
}
}
