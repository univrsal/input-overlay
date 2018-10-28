/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "gamepad.hpp"
#include <stdio.h>

namespace gamepad
{
	bool gamepad_hook_state = false;
	bool gamepad_hook_run_flag = true;
	gamepad_state pad_states[PAD_COUNT];

#ifdef _WIN32
	static HANDLE hook_thread;
#else
	static pthread_t game_pad_hook_thread;
#endif

    gamepad_state::~gamepad_state()
    {
		unload();
    }

    void gamepad_state::unload()
    {
#ifdef _WIN32
		RtlZeroMemory(&m_x_input, sizeof(xinput_fix::gamepad));
#else
		if (m_device_file)
			fclose(m_device_file);
		m_device_file = nullptr;
#endif
    }

    void gamepad_state::load()
    {
#ifdef _WIN32
		unload();
		update();
#else
		m_device_file = fopen(m_path.c_str(), "wb+");
		if (m_device_file)
		{
			void * tmp = malloc(8 * 12 * sizeof(char));
			fread(tmp, sizeof(char) * 8 * 12, 1, m_device_file);
			free(tmp);
		}
#endif
    }

    bool gamepad_state::valid()
    {
#ifdef _WIN32
		update();
		return m_valid;
#else
		return m_device_file != nullptr && m_pad_id >= 0;
#endif
    }

    void gamepad_state::init(const uint8_t pad_id)
    {
#ifdef _WIN32
		m_pad_id = pad_id;
		load();
#else
		unload();
		m_pad_id = pad_id;
		m_path.clear();
		m_path = "/dev/input/js";
		m_path.append(std::to_string(pad_id));
		load();
#endif
    }

    uint8_t gamepad_state::get_id() const
    {
		return m_pad_id;
    }

#ifdef _WIN32
    void gamepad_state::update()
    {
		if (xinput_fix::update(m_pad_id, &m_x_input) == ERROR_SUCCESS)
			m_valid = true;
		else
			m_valid = false;
    }

#else
	FILE * gamepad_state::dev()
	{
		return m_device_file;
	}
#endif

    /* Hook util methods */

	void start_pad_hook()
	{
		if (gamepad_hook_state)
			return;

#ifdef _WIN32
		xinput_fix::load();

		if (!xinput_fix::loaded) /* Couldn't load xinput Dll*/
		{
			printf("Gamepad hook init failed\n");
			return;
		}
#endif
		gamepad_hook_state = gamepad_hook_run_flag = init_pads();

#ifdef _WIN32
		hook_thread = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(hook_method),
			nullptr, 0, nullptr);
		gamepad_hook_state = hook_thread;
#else
		gamepad_hook_state = pthread_create(&game_pad_hook_thread, nullptr, hook_method, nullptr) == 0;
#endif
	}

	bool init_pads()
	{
		uint8_t id = 0;
		auto flag = false;
		for (auto& state : pad_states)
		{
			state.init(id++);
			if (state.valid())
				flag = true;
		}
		return flag;
	}

	void end_pad_hook()
	{
		gamepad_hook_run_flag = false;

#ifdef _WIN32
		CloseHandle(hook_thread);
#endif
	}
	/* Background process for quering game pads */
#ifdef _WIN32
	DWORD WINAPI hook_method(const LPVOID arg)
#else
	void* hook_method(void *)
#endif
    {
		while (gamepad_hook_run_flag)
		{
			for (auto& pad : pad_states)
			{
				if (!pad.valid())
					continue;
#ifdef _WIN32

#else

#endif
			}
		}

#ifdef _WIN32
		return 0x0;
#else
		pthread_exit(nullptr);
#endif
    }
}
