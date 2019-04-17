/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "hook-helper.hpp"

void util_clear_pressed(void)
{
    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
        pressed_keys[i] = 0;
}

bool util_pressed_empty(void)
{
    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++) {
        if (pressed_keys[i] > 0)
            return false;
    }
    return true;
}

bool util_key_exists(uint16_t vc)
{
    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++) {
        if (pressed_keys[i] == vc)
            return true;
    }
    return false;
}

void util_add_pressed(uint16_t vc)
{
    if (!util_key_exists(vc)) {
        for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++) {
            if (pressed_keys[i] == 0) {
                pressed_keys[i] = vc;
                break;
            }
        }
    }
}

void util_remove_pressed(uint16_t vc)
{
    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++) {
        if (pressed_keys[i] == vc) {
            pressed_keys[i] = VC_UNDEFINED;
            break;
        }
    }
}

/*
    Hook process stuff 
    Source:    
    https://github.com/kwhat/libuiohook/blob/master/src/demo_hook_async.c
*/

uint16_t pressed_keys[MAX_SIMULTANEOUS_KEYS];
int16_t mouse_x, mouse_y, mouse_x_smooth, mouse_y_smooth, mouse_last_x, mouse_last_y;
bool hook_initialized = false;

#ifdef WINDOWS
static HANDLE hook_thread;
static HANDLE hook_running_mutex;
static HANDLE hook_control_mutex;
static HANDLE hook_control_cond;
#else
static pthread_t hook_thread;
static pthread_mutex_t hook_running_mutex;
static pthread_mutex_t hook_control_mutex;
static pthread_cond_t hook_control_cond;
#endif

void dispatch_proc(uiohook_event* const event)
{
    switch (event->type) {
        case EVENT_HOOK_ENABLED:
#ifdef WINDOWS
            WaitForSingleObject(hook_running_mutex, INFINITE);
#else
            pthread_mutex_lock(&hook_running_mutex);
#endif

#ifdef WINDOWS
            SetEvent(hook_control_cond);
#else
            pthread_cond_signal(&hook_control_cond);
            pthread_mutex_unlock(&hook_control_mutex);
#endif
            break;
        case EVENT_HOOK_DISABLED:
#ifdef WINDOWS
            WaitForSingleObject(hook_control_mutex, INFINITE);
#else
            pthread_mutex_lock(&hook_control_mutex);
#endif

#ifdef WINDOWS
        ReleaseMutex(hook_running_mutex);
        ResetEvent(hook_control_cond);
#else
#if defined(__APPLE__) && defined(__MACH__)
            CFRunLoopStop(CFRunLoopGetMain());
#endif
            pthread_mutex_unlock(&hook_running_mutex);
#endif
    }
    proccess_event(event);
}

#ifdef WINDOWS
DWORD WINAPI hook_thread_proc(LPVOID arg)
{
    // Set the hook status.
    int status = hook_run();
    if (status != UIOHOOK_SUCCESS) {
        *(DWORD *)arg = status;
        *(int *)arg = status;
    }

    SetEvent(hook_control_cond);
    return status;
}
#else

void* hook_thread_proc(void* arg)
{
    int status = hook_run();
    if (status != UIOHOOK_SUCCESS) {
        *(int*) arg = status;
    }
    pthread_cond_signal(&hook_control_cond);
    pthread_mutex_unlock(&hook_control_mutex);

    return arg;
}

#endif

bool logger_proc(unsigned int level, const char* format, ...)
{
    va_list args;
    switch (level) {
        case LOG_LEVEL_WARN:
        case LOG_LEVEL_ERROR:
            va_start(args, format);
            std::string f(format);
            f.insert(0, "[input-overlay] ");
            blog(LOG_WARNING, f.c_str(), args);
            va_end(args);
    }
    return true;
}

void end_hook(void)
{
#ifdef WINDOWS
    // Create event handles for the thread hook.
    CloseHandle(hook_thread);
    CloseHandle(hook_running_mutex);
    CloseHandle(hook_control_mutex);
    CloseHandle(hook_control_cond);
#else
    pthread_mutex_destroy(&hook_running_mutex);
    pthread_mutex_destroy(&hook_control_mutex);
    pthread_cond_destroy(&hook_control_cond);
#endif
    hook_stop();
}

void start_hook(void)
{
#ifdef _WIN32
    hook_running_mutex = CreateMutex(NULL, FALSE, TEXT("hook_running_mutex"));
    hook_control_mutex = CreateMutex(NULL, FALSE, TEXT("hook_control_mutex"));
    hook_control_cond = CreateEvent(NULL, TRUE, FALSE, TEXT("hook_control_cond"));
#else
    pthread_mutex_init(&hook_running_mutex, NULL);
    pthread_mutex_init(&hook_control_mutex, NULL);
    pthread_cond_init(&hook_control_cond, NULL);
#endif

    // Set the logger callback for library output.
    hook_set_logger_proc(&logger_proc);

    // Set the event callback for uiohook events.
    hook_set_dispatch_proc(&dispatch_proc);

    int status = hook_enable();
    switch (status) {
        case UIOHOOK_SUCCESS:
            // We no longer block, so we need to explicitly wait for the thread to die.
            hook_initialized = true;
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

void proccess_event(uiohook_event* const event)
{
    util_remove_pressed(VC_MOUSE_WHEEL_UP);
    util_remove_pressed(VC_MOUSE_WHEEL_DOWN);

    switch (event->type) {
        case EVENT_KEY_PRESSED:
            if (!util_key_exists(event->data.keyboard.keycode))
                util_add_pressed(event->data.keyboard.keycode);
            break;
        case EVENT_KEY_RELEASED:
            if (util_key_exists(event->data.keyboard.keycode))
                util_remove_pressed(event->data.keyboard.keycode);
            break;
        case EVENT_MOUSE_PRESSED:
            if (!util_key_exists(util_mouse_to_vc(event->data.keyboard.keycode)))
                util_add_pressed(util_mouse_to_vc(event->data.keyboard.keycode));
            break;
        case EVENT_MOUSE_RELEASED:
            if (util_key_exists(util_mouse_to_vc(event->data.mouse.button)))
                util_remove_pressed(util_mouse_to_vc(event->data.mouse.button));
            break;
        case EVENT_MOUSE_WHEEL:
            if (event->data.wheel.rotation == WHEEL_UP) {
                if (!util_key_exists(VC_MOUSE_WHEEL_UP)) {
                    util_add_pressed(VC_MOUSE_WHEEL_UP);
                }
            } else if (event->data.wheel.rotation == WHEEL_DOWN) {
                if (!util_key_exists(VC_MOUSE_WHEEL_DOWN)) {
                    util_add_pressed(VC_MOUSE_WHEEL_DOWN);
                }
            }
            break;
        case EVENT_MOUSE_DRAGGED:
        case EVENT_MOUSE_MOVED:
            mouse_last_x = mouse_x;
            mouse_last_y = mouse_y;
            mouse_x = event->data.mouse.x;
            mouse_y = event->data.mouse.y;
            mouse_x_smooth = (uint16_t)((mouse_last_x * 4 + mouse_x + 4) / 5);
            mouse_y_smooth = (uint16_t)((mouse_last_y * 4 + mouse_y + 4) / 5);
            break;
    }
}

int hook_enable(void)
{
    // Lock the thread control mutex.  This will be unlocked when the
    // thread has finished starting, or when it has fully stopped.
#ifdef WINDOWS
    WaitForSingleObject(hook_control_mutex, INFINITE);
#else
    pthread_mutex_lock(&hook_control_mutex);
#endif

    // Set the initial status.
    int status = UIOHOOK_FAILURE;

#ifndef WINDOWS
    // Create the thread attribute.
    pthread_attr_t hook_thread_attr;
    pthread_attr_init(&hook_thread_attr);

    // Get the policy and priority for the thread attr.
    int policy;
    pthread_attr_getschedpolicy(&hook_thread_attr, &policy);
    int priority = sched_get_priority_max(policy);
#endif

#if defined(WINDOWS)
    DWORD hook_thread_id;
    DWORD *hook_thread_status = (DWORD*) malloc(sizeof(DWORD));
    hook_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hook_thread_proc, hook_thread_status, 0, &hook_thread_id);
    if (hook_thread != INVALID_HANDLE_VALUE)
    {
#else
    int* hook_thread_status = (int*) malloc(sizeof(int));
    if (pthread_create(&hook_thread, &hook_thread_attr, hook_thread_proc, hook_thread_status) == 0) {
#endif
#if defined(WINDOWS)
        // Attempt to set the thread priority to time critical.
        if (SetThreadPriority(hook_thread, THREAD_PRIORITY_TIME_CRITICAL) == 0)
        {
            blog(LOG_WARNING, "[input-overlay] %s [%u]: Could not set thread priority %li for hook thread %#p! (%#lX)\n",
                __FUNCTION__, __LINE__, (long) THREAD_PRIORITY_TIME_CRITICAL, hook_thread, (unsigned long) GetLastError());
        }
#elif (defined(__APPLE__) && defined(__MACH__)) || _POSIX_C_SOURCE >= 200112L
        // Some POSIX revisions do not support pthread_setschedprio so we will
        // use pthread_setschedparam instead.
        struct sched_param param = {.sched_priority = priority};
        if (pthread_setschedparam(hook_thread, SCHED_OTHER, &param) != 0) {
            blog(LOG_WARNING, "[input-overlay] %s [%u]: Could not set thread priority %i for thread 0x%lX!\n",
                 __FUNCTION__, __LINE__, priority, (unsigned long) hook_thread);
        }
#else
        // Raise the thread priority using glibc pthread_setschedprio.
        if (pthread_setschedprio(hook_thread, priority) != 0)
        {
            blog(LOG_WARNING, "[input-overlay] %s [%u]: Could not set thread priority %i for thread 0x%lX!\n",
                __FUNCTION__, __LINE__, priority, (unsigned long)hook_thread);
        }
#endif
        // Wait for the thread to indicate that it has passed the
        // initialization portion by blocking until either a EVENT_HOOK_ENABLED
        // event is received or the thread terminates.
        // NOTE This unlocks the hook_control_mutex while we wait.
#ifdef WINDOWS
        WaitForSingleObject(hook_control_cond, INFINITE);
#else
        pthread_cond_wait(&hook_control_cond, &hook_control_mutex);
#endif

#ifdef WINDOWS
        if (WaitForSingleObject(hook_running_mutex, 0) != WAIT_TIMEOUT)
        {
#else
        if (pthread_mutex_trylock(&hook_running_mutex) == 0) {
#endif
            // Lock Successful; The hook is not running but the hook_control_cond
            // was signaled!  This indicates that there was a startup problem!

            // Get the status back from the thread.
#ifdef WINDOWS
            WaitForSingleObject(hook_thread, INFINITE);
            GetExitCodeThread(hook_thread, hook_thread_status);
#else
            pthread_join(hook_thread, (void**) &hook_thread_status);
            status = *hook_thread_status;
#endif
        } else {
            // Lock Failure; The hook is currently running and wait was signaled
            // indicating that we have passed all possible start checks.  We can
            // always assume a successful startup at this point.
            status = UIOHOOK_SUCCESS;
        }

        free(hook_thread_status);

        blog(LOG_DEBUG, "[input-overlay] %s [%u]: Thread Result: (%#X).\n", __FUNCTION__, __LINE__, status);
    } else {
        status = -1;
    }

    // Make sure the control mutex is unlocked.
#ifdef WINDOWS
    ReleaseMutex(hook_control_mutex);
#else
    pthread_mutex_unlock(&hook_control_mutex);
#endif

    return status;
}