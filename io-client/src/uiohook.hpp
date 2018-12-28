/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <uiohook.h>
#include <memory>
#include <mutex>
#include <queue>

namespace uiohook
{
    class event_holder
    {
        std::mutex m_mutex;
        uiohook_event* m_event = nullptr;
    public:
        event_holder();
        ~event_holder();
        uiohook_event* get() const;
        void process_event(uiohook_event * event);
    };

    extern volatile bool new_event;
    extern event_holder holder;
    extern volatile bool hook_state;
	bool logger_proc(unsigned level, const char* format, ...);

	void dispatch_proc(uiohook_event * event);
    void force_refresh(); /* Server requested update */
	bool init();
    void close();
}
