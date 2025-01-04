/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
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
#include <cstdarg>
#include <obs-module.h>
#include <obs/obs-nix-platform.h>
#include <uiohook.h>

namespace uiohook {
/*
        Hook process stuff
        Source:
        https://github.com/kwhat/libuiohook/blob/master/src/demo_hook_async.c
*/

uint64_t last_scroll_time = 0; /* System time at last scroll event */
bool state = false;
std::mutex data_mutex;

static pthread_t hook_thread;
static pthread_mutex_t hook_running_mutex;
static pthread_mutex_t hook_control_mutex;
static pthread_cond_t hook_control_cond;

void *hook_thread_proc(void *arg)
{
    int status = hook_run();
    if (status != UIOHOOK_SUCCESS) {
        *(int *)arg = status;
    }
    pthread_cond_signal(&hook_control_cond);
    pthread_mutex_unlock(&hook_control_mutex);

    return arg;
}

extern "C" {
static void logger_proc(unsigned int level, void *, const char *format, va_list args)
{
    switch (level) {
    default:
    case LOG_LEVEL_DEBUG:
        blogva(LOG_DEBUG, format, args);
        break;
    case LOG_LEVEL_INFO:
        blogva(LOG_INFO, format, args);
        break;
    case LOG_LEVEL_WARN:
    case LOG_LEVEL_ERROR:
        blogva(LOG_WARNING, format, args);
        break;
    }
}
}

void dispatch_proc(uiohook_event *event, void *)
{
    switch (event->type) {
    case EVENT_HOOK_ENABLED:
        pthread_mutex_lock(&hook_running_mutex);

        pthread_cond_signal(&hook_control_cond);
        pthread_mutex_unlock(&hook_control_mutex);
        break;
    case EVENT_HOOK_DISABLED:
        pthread_mutex_lock(&hook_control_mutex);
        pthread_mutex_unlock(&hook_running_mutex);
    default:; /* Prevent missing case error */
    }
    process_event(event);
}

int hook_enable()
{
    /* Lock the thread control mutex.  This will be unlocked when the
       thread has finished starting, or when it has fully stopped. */
    pthread_mutex_lock(&hook_control_mutex);

    /* Set the initial status. */
    int status = UIOHOOK_FAILURE;

    /* Create the thread attribute. */
    pthread_attr_t hook_thread_attr;
    pthread_attr_init(&hook_thread_attr);

    /* Get the policy and priority for the thread attr. */
    int policy;
    pthread_attr_getschedpolicy(&hook_thread_attr, &policy);
    int priority = sched_get_priority_max(policy);

    int *hook_thread_status = (int *)malloc(sizeof(int));
    if (pthread_create(&hook_thread, &hook_thread_attr, hook_thread_proc, hook_thread_status) == 0) {
        /* Some POSIX revisions do not support pthread_setschedprio so we will
         * use pthread_setschedparam instead. */
        struct sched_param param = {.sched_priority = priority};
        if (pthread_setschedparam(hook_thread, SCHED_OTHER, &param) != 0) {
            blog(LOG_WARNING, "[input-overlay] %s [%u]: Could not set thread priority %i for thread 0x%lX!",
                 __FUNCTION__, __LINE__, priority, (unsigned long)hook_thread);
        }
        /* Wait for the thread to indicate that it has passed the
           initialization portion by blocking until either a EVENT_HOOK_ENABLED
           event is received or the thread terminates.
           NOTE This unlocks the hook_control_mutex while we wait.*/

        pthread_cond_wait(&hook_control_cond, &hook_control_mutex);

        if (pthread_mutex_trylock(&hook_running_mutex) == 0) {
            /* Lock Successful; The hook is not running but the hook_control_cond
               was signaled!  This indicates that there was a startup problem!
               Get the status back from the thread. */
            pthread_join(hook_thread, (void **)&hook_thread_status);
            status = *hook_thread_status;
        } else {
            /* Lock Failure; The hook is currently running and wait was signaled
                   indicating that we have passed all possible start checks.  We can
                   always assume a successful startup at this point. */
            status = UIOHOOK_SUCCESS;
        }

        free(hook_thread_status);

        blog(LOG_DEBUG, "[input-overlay] uiohook thread result: %#X", status);
    } else {
        status = -1;
    }

    /* Make sure the control mutex is unlocked. */
    pthread_mutex_unlock(&hook_control_mutex);

    return status;
}

void stop()
{
    pthread_mutex_destroy(&hook_running_mutex);
    pthread_mutex_destroy(&hook_control_mutex);
    pthread_cond_destroy(&hook_control_cond);
}

void start()
{
    // Check if we're running on wayland
    if (obs_get_nix_platform() != OBS_NIX_PLATFORM_X11_EGL) {
        blog(LOG_WARNING,
             "[input-overlay] Wayland is not supported by libuiohook. Keyboard an mouse hook will not work.\n");
        return;
    }

    pthread_mutex_init(&hook_running_mutex, nullptr);
    pthread_mutex_init(&hook_control_mutex, nullptr);
    pthread_cond_init(&hook_control_cond, nullptr);

    /* Set the logger callback for library output. */
    hook_set_logger_proc(&logger_proc, nullptr);

    /* Set the event callback for uiohook events. */
    hook_set_dispatch_proc(&dispatch_proc, nullptr);

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
