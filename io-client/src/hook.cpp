/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "hook.hpp"
#include <cstdarg>
#include <cstdio>

namespace hook
{
    bool logger_proc(unsigned level, const char* format, ...)
    {
        bool status = false;

        va_list args;
        switch (level)
        {
#ifdef USE_DEBUG
		default:
		case LOG_LEVEL_DEBUG:
		case LOG_LEVEL_INFO:
			va_start(args, format);
			status = vfprintf(stdout, format, args) >= 0;
			va_end(args);
			break;
#endif
        case LOG_LEVEL_WARN:
        case LOG_LEVEL_ERROR:
            va_start(args, format);
            status = vfprintf(stderr, format, args) >= 0;
            va_end(args);
            break;
        }

        return status;
    }

	void dispatch_proc(uiohook_event* const event)
	{
		switch (event->type)
		{
		case EVENT_KEY_PRESSED:

		case EVENT_KEY_RELEASED:

			break;

		case EVENT_KEY_TYPED:

			break;

		case EVENT_MOUSE_PRESSED:
		case EVENT_MOUSE_RELEASED:
		case EVENT_MOUSE_CLICKED:
		case EVENT_MOUSE_MOVED:
		case EVENT_MOUSE_DRAGGED:
			break;

		case EVENT_MOUSE_WHEEL:

			break;

		default:
			break;
		}
	}

    bool init()
    {
		hook_set_logger_proc(&logger_proc);
    	hook_set_dispatch_proc(&dispatch_proc);

		const auto status = hook_run();

		switch (status)
		{
		case UIOHOOK_SUCCESS:
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
		default: ;
		}
    }

	void close()
    {
		const auto status = hook_stop();
		printf("Closing hook\n");
		switch (status)
		{
		case UIOHOOK_ERROR_OUT_OF_MEMORY:
			logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to allocate memory. (%#X)", status);
			break;
		case UIOHOOK_ERROR_X_RECORD_GET_CONTEXT:
			logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to get XRecord context. (%#X)", status);
			break;
		case UIOHOOK_FAILURE:
			logger_proc(LOG_LEVEL_ERROR, "[uiohook] An unknown hook error occurred. (%#X)", status);
			break;
		default: ;
		}
    }
}
