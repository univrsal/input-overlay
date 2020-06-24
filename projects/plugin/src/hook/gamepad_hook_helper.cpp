#include "gamepad_hook_helper.hpp"

namespace gamepad {
    ;
bool gamepad_hook_state = false;
	std::mutex mutex;
	bool gamepad_hook_run_flag = false;

void start_pad_hook() {}

void end_pad_hook()
{
}

bool init_pad_devices()
{
	return false;
}
}