/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "gamepad.hpp"
#include "network.hpp"
#include <stdio.h>
#include <thread>

namespace gamepad
{
    volatile bool hook_state = false;
    volatile bool hook_run_flag = true;
	gamepad_handle pad_handles[PAD_COUNT];

#ifdef _WIN32
    HANDLE hook_thread;
#else
    pthread_t game_pad_hook_thread;
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

    void gamepad_handle::update_state(gamepad_state * new_state)
    {
        if (new_state)
        {
            if (m_current_state.merge(new_state))
            {
                m_changed = true;
                network::data_to_send = true;
            }
        }
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

	bool start_pad_hook(bool threaded)
	{
		if (hook_state)
			return true;

#ifdef _WIN32
		xinput_fix::load();

		if (!xinput_fix::loaded) /* Couldn't load xinput Dll*/
		{
			printf("Xinput init failed\n");
			return false;
		}
#endif
		hook_state = hook_run_flag = init_pads();
        if (!hook_state)
        {
            printf("Initializing gamepads failed\n");
            return false;
        }

        if (threaded)
        {
#ifdef _WIN32
		    hook_thread = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(hook_method),
			    nullptr, 0, nullptr);
		    hook_state = hook_thread;
#else
            hook_state = pthread_create(&game_pad_hook_thread, nullptr, hook_method, nullptr) == 0;
#endif
        }
        else
        {
            hook_state = true;
            hook_method(nullptr);
        }
        return hook_state;
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
#ifdef _WIN32
        return true; /* On windows we can use hotplug and detect gamepads on the fly */
#else
        return flag;
#endif
		
	}

	void close()
	{
		if (!hook_state)
			return;
		hook_state = false;
		hook_run_flag = false;
	}

    bool check_changes()
    {
        if (!util::cfg.monitor_gamepad)
            return false;
        for (auto& pad : pad_handles)
            if (pad.m_changed)
                return true;
        return false;
    }

	/* Background process for querying game pads */
#ifdef _WIN32
	DWORD WINAPI hook_method(const LPVOID arg)
#else
    void* hook_method(void *)
#endif
    {
        /* The hook only keeps track of the gamepad states here
         * and the changes will then be sent in a different thread
         */
		while (hook_run_flag)
		{
#ifdef _WIN32 /* On linux the check happens inside the for loop */
            if (network::data_block)
            {
                Sleep(25);
                continue;
            }
#endif
            for (auto& pad : pad_handles)
            {
                if (!pad.valid())
                    continue;
#ifdef _WIN32
                gamepad_state new_state(pad.get_xinput());
                pad.update_state(&new_state);
#else
                /* TODO: Linux implementation */
#endif
            }
#ifdef _WIN32
            Sleep(25);
#endif
		}

        for (auto& pad : pad_handles)
            pad.unload();

#ifdef _WIN32
		return 0x0;
#else
		pthread_exit(nullptr);
#endif
    }
}
