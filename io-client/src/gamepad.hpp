/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <stdint.h>
#include <mutex>
#include "gamepad_state.hpp"
#include "xinput_fix.hpp"

#define PAD_COUNT 4

namespace gamepad
{
#ifdef _WIN32
    extern HANDLE hook_thread;
#else
    extern pthread_t game_pad_hook_thread;
#endif
    class gamepad_handle;
    extern gamepad_handle pad_handles[PAD_COUNT];
    extern volatile bool hook_state;
    extern volatile bool hook_run_flag;

    static xinput_fix::gamepad_codes pad_keys[] =
	{ /* These keycodes are only used on windows,
	     but the linux client converts them to these
	     to agree on one standard 
	  */
		xinput_fix::CODE_A,
		xinput_fix::CODE_B,
		xinput_fix::CODE_X,
		xinput_fix::CODE_Y,
        xinput_fix::CODE_GUIDE,
        xinput_fix::CODE_DPAD_DOWN,
        xinput_fix::CODE_DPAD_UP,
		xinput_fix::CODE_DPAD_LEFT,
		xinput_fix::CODE_DPAD_RIGHT,
		xinput_fix::CODE_LEFT_SHOULDER,
		xinput_fix::CODE_RIGHT_SHOULDER,
		xinput_fix::CODE_START,
		xinput_fix::CODE_BACK
	};
#ifndef _WIN32
#define ID_TYPE         6
#define ID_BUTTON       1
#define ID_STATE_1      4
#define ID_STATE_2      5
#define ID_KEY_CODE     7
#define ID_PRESSED      1

#define ID_L_ANALOG_X   0
#define ID_L_ANALOG_Y   1
#define ID_L_TRIGGER    2
#define ID_R_ANALOG_X   3
#define ID_R_ANALOG_Y   4
#define ID_R_TRIGGER    5
#endif /* !WINDOWS */

	class gamepad_handle
	{
	public:
		~gamepad_handle();

		void unload();

		void load();

		bool valid();

		void init(const uint8_t pad_id);

		uint8_t get_id() const;

        gamepad_state* get_state();

        void update_state(gamepad_state* new_state);

        bool m_changed = false;
#ifdef _WIN32
		void update();
		xinput_fix::gamepad* get_xinput();
	private:
        std::mutex m_mutex;
		xinput_fix::gamepad m_x_input = {};
		bool m_valid = false;
#else
		FILE* dev();

	private:
		FILE* m_device_file = nullptr;
		std::string m_path;
#endif
		int8_t m_pad_id = -1;
		gamepad_state m_current_state;
        
    };

    /* Thread stuff*/
	bool start_pad_hook(bool threaded);
	bool init_pads();
	void close();

    bool check_changes();

#ifdef _WIN32
	DWORD WINAPI hook_method(LPVOID arg);
#else
	void* hook_method(void*);
#endif


} /* namespace gamepad */
