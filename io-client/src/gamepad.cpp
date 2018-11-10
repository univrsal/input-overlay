/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "gamepad.hpp"
#include <stdio.h>
#include "network.hpp"

namespace gamepad
{
	bool gamepad_hook_state = false;
	bool gamepad_hook_run_flag = true;
	gamepad_handle pad_handles[PAD_COUNT];

#ifdef _WIN32
	static HANDLE hook_thread;
#else
	static pthread_t game_pad_hook_thread;
#endif

	gamepad_handle::~gamepad_handle()
    {
		unload();
    }

    void gamepad_handle::unload()
    {
#ifdef _WIN32
		RtlZeroMemory(&m_x_input, sizeof(xinput_fix::gamepad));
#else
		if (m_device_file)
			fclose(m_device_file);
		m_device_file = nullptr;
#endif
    }

    void gamepad_handle::load()
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

    bool gamepad_handle::valid()
    {
#ifdef _WIN32
		update();
		return m_valid;
#else
		return m_device_file != nullptr && m_pad_id >= 0;
#endif
    }

    void gamepad_handle::init(const uint8_t pad_id)
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

    uint8_t gamepad_handle::get_id() const
    {
		return m_pad_id;
    }

    gamepad_state* gamepad_handle::get_state()
    {
        return &m_current_state;
    }

    void gamepad_handle::set_state(const gamepad_state new_state)
    {
        m_current_state = new_state;
    }

#ifdef _WIN32
    void gamepad_handle::update()
    {
		if (xinput_fix::update(m_pad_id, &m_x_input) == ERROR_SUCCESS)
			m_valid = true;
		else
			m_valid = false;
    }

    xinput_fix::gamepad* gamepad_handle::get_xinput()
    {
		return &m_x_input;
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
		for (auto& state : pad_handles)
		{
			state.init(id++);
			if (state.valid())
				flag = true;
		}
		return flag;
	}

	void end_pad_hook()
	{
		if (!gamepad_hook_state)
			return;
		gamepad_hook_state = false;
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
        /* The hook only keeps track of the gamepad states here
         * and the changes will then be sent in a different thread
         */
		while (gamepad_hook_run_flag)
		{
			for (auto& pad : pad_handles)
			{
				if (!pad.valid())
					continue;
#ifdef _WIN32
                gamepad_state new_state(pad.get_xinput());
                if (pad.get_state()->merge(&new_state))
                {
                    pad.set_state(new_state);
                }
#else
                /* TODO: Linux implementation */
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
