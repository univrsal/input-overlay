/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <stdint.h>
#define PAD_COUNT 4

namespace gamepad
{
#ifdef _WIN32
#include "xinput_fix.hpp"
	static xinput_fix::gamepad_codes pad_keys[] =
	{
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
#else
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
#endif /* WINDOWS */

	class gamepad_handle
	{
	public:
		~gamepad_handle();

		void unload();

		void load();

		bool valid();

		void init(const uint8_t pad_id);

		uint8_t get_id() const;

#ifdef _WIN32
		void update();
		xinput_fix::gamepad* get_xinput();
	private:
		xinput_fix::gamepad m_x_input;
		bool m_valid = false;
#else
		FILE* dev();

	private:
		FILE* m_device_file = nullptr;
		std::string m_path;
#endif
		int8_t m_pad_id = -1;
		gamepad_state old_state;
    };

    /* Thread stuff*/
	void start_pad_hook();
	bool init_pads();
	void end_pad_hook();

	extern gamepad_handle pad_handles[PAD_COUNT];
	extern bool hook_state;
	extern bool hook_run_flag;

#ifdef _WIN32
	DWORD WINAPI hook_method(LPVOID arg);
#else
	void* hook_method(void*);
#endif


} /* namespace gamepad */