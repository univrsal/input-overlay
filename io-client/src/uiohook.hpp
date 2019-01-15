/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <uiohook.h>
#include <mutex>
#include <map>
#include <netlib.h>

namespace uiohook
{
    class data_holder
    {
        std::mutex m_mutex;
        std::map<uint16_t, bool> m_button_states;
        int16_t m_mouse_x, m_mouse_y;
        int8_t m_wheel_direction;
        uint16_t m_lmb_clicks, m_rmb_clicks, m_mmb_clicks;
        bool m_new_mouse_data;
    public:
        data_holder();
        void add_button(uint16_t keycode, bool pressed);
        void add_mouse(int16_t x, int16_t y);
        bool write_to_buffer(netlib_byte_buf* buffer);
    };

    extern data_holder data;
    extern volatile bool hook_state;
	bool logger_proc(unsigned level, const char* format, ...);

	void dispatch_proc(uiohook_event * event);
    bool init();
    void close();
}
