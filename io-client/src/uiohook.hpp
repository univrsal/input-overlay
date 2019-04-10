/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <uiohook.h>
#include <mutex>
#include <map>
#include <netlib.h>

#define SCROLL_TIMEOUT 120
namespace uiohook
{
    extern std::mutex m_mutex;
    enum wheel_dir
    {
        wheel_up = -1,
        wheel_none,
        wheel_down
    };


    class data_holder
    {
        std::map<uint16_t, bool> m_button_states;
        int16_t m_mouse_x, m_mouse_y;
        wheel_dir m_wheel_direction;
        int16_t m_wheel_amount;
        bool m_wheel_pressed;
        bool m_new_mouse_data;
        uint32_t m_last_scroll;

    public:
        data_holder();
        void set_button(uint16_t keycode, bool pressed);
        void set_mouse_pos(int16_t x, int16_t y);
        void set_wheel(int amount, wheel_dir dir);
        void reset_wheel();
        void set_wheel(bool pressed);
        bool write_to_buffer(netlib_byte_buf* buffer);
        uint32_t get_last_scroll();
    };

    inline uint16_t util_mouse_fix(int m)
    {
#ifndef _WIN32 /* Linux mixes right mouse and middle mouse or is windows getting it wrong? */
        if (m == 3)
            m = 2;
        else if (m == 2)
            m = 3;
#endif
        return m;
    }
    
    inline bool is_middle_mouse(int m)
    {
        return util_mouse_fix(m) == MOUSE_BUTTON3;
    }

    extern data_holder data;
    extern volatile bool hook_state;
	bool logger_proc(unsigned level, const char* format, ...);

	void dispatch_proc(uiohook_event * event);
    bool init();
    void close();
}
