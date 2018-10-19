/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <uiohook.h>

namespace hook
{
	
	bool logger_proc(unsigned level, const char* format, ...);

	void dispatch_proc(uiohook_event * const event);

	bool init();

	void close();
}