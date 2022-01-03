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
#include <cstdarg>
#include <obs-module.h>

namespace uiohook {
/*
    Hook process stuff
    Source:
    https://github.com/kwhat/libuiohook/blob/master/src/demo_hook_async.c
*/

uint64_t last_scroll_time = 0; /* System time at last scroll event */
bool state = false;
std::mutex data_mutex;

static HANDLE hook_thread;
static HANDLE hook_running_mutex;
static HANDLE hook_control_mutex;
static HANDLE hook_control_cond;

void dispatch_proc(uiohook_event *const event)
{
    switch (event->type) {
    case EVENT_HOOK_ENABLED:

        WaitForSingleObject(hook_running_mutex, INFINITE);
        SetEvent(hook_control_cond);
        break;
    case EVENT_HOOK_DISABLED:
        WaitForSingleObject(hook_control_mutex, INFINITE);
        ReleaseMutex(hook_running_mutex);
        ResetEvent(hook_control_cond);
    default:; /* Prevent missing case error */
    }
    process_event(event);
}

DWORD WINAPI hook_thread_proc(const LPVOID arg)
{
    /* Set the hook status. */
    const auto status = hook_run();
    if (status != UIOHOOK_SUCCESS) {
        *static_cast<DWORD *>(arg) = status;
        *static_cast<int *>(arg) = status;
    }

    SetEvent(hook_control_cond);
    return status;
}

bool logger_proc(const unsigned int level, const char *format, ...)
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

void stop()
{
    CloseHandle(hook_thread);
    CloseHandle(hook_running_mutex);
    CloseHandle(hook_control_mutex);
    CloseHandle(hook_control_cond);
}

int hook_enable()
{
    /* Lock the thread control mutex.  This will be unlocked when the
       thread has finished starting, or when it has fully stopped. */

    WaitForSingleObject(hook_control_mutex, INFINITE);

    /* Set the initial status. */
    int status = UIOHOOK_FAILURE;

    DWORD hook_thread_id;
    DWORD *hook_thread_status = (DWORD *)malloc(sizeof(DWORD));
    hook_thread =
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)hook_thread_proc, hook_thread_status, 0, &hook_thread_id);
    if (hook_thread != INVALID_HANDLE_VALUE) {

        /* Attempt to set the thread priority to time critical. */
        if (SetThreadPriority(hook_thread, THREAD_PRIORITY_TIME_CRITICAL) == 0) {
            blog(LOG_WARNING,
                 "[input-overlay] %s [%u]: Could not set thread priority %li for hook thread %#p! (%#lX)\n",
                 __FUNCTION__, __LINE__, (long)THREAD_PRIORITY_TIME_CRITICAL, hook_thread,
                 (unsigned long)GetLastError());
        }

        /* Wait for the thread to indicate that it has passed the
         * initialization portion by blocking until either a EVENT_HOOK_ENABLED
         * event is received or the thread terminates.
         * NOTE This unlocks the hook_control_mutex while we wait.*/

        WaitForSingleObject(hook_control_cond, INFINITE);

        if (WaitForSingleObject(hook_running_mutex, 0) != WAIT_TIMEOUT) {

            /* Lock Successful; The hook is not running but the hook_control_cond
             * was signaled!  This indicates that there was a startup problem!
             * Get the status back from the thread. */
            WaitForSingleObject(hook_thread, INFINITE);
            GetExitCodeThread(hook_thread, hook_thread_status);

        } else {
            /* Lock Failure; The hook is currently running and wait was signaled
             * indicating that we have passed all possible start checks.  We can
             * always assume a successful startup at this point. */
            status = UIOHOOK_SUCCESS;
        }

        free(hook_thread_status);

        blog(LOG_DEBUG, "[input-overlay] %s [%u]: Thread Result: (%#X).\n", __FUNCTION__, __LINE__, status);
    } else {
        status = -1;
    }

    /* Make sure the control mutex is unlocked. */
    ReleaseMutex(hook_control_mutex);
    return status;
}

void start()
{
    hook_running_mutex = CreateMutex(nullptr, FALSE, TEXT("hook_running_mutex"));
    hook_control_mutex = CreateMutex(nullptr, FALSE, TEXT("hook_control_mutex"));
    hook_control_cond = CreateEvent(nullptr, TRUE, FALSE, TEXT("hook_control_cond"));

    /* Set the logger callback for library output. */
    hook_set_logger_proc(&logger_proc);

    /* Set the event callback for uiohook events. */
    hook_set_dispatch_proc(&dispatch_proc);

    const auto status = hook_enable();
    switch (status) {
    case UIOHOOK_SUCCESS:
        /* We no longer block, so we need to explicitly wait for the thread to die. */
        state = true;
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
}
