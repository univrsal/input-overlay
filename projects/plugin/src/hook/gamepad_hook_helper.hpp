/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#pragma once

#include <mutex>
#include <memory>

namespace gamepad {
class hook;
}

namespace libgamepad {
extern uint16_t flags;
extern uint16_t last_input;
extern uint16_t last_input_value;
extern uint64_t last_input_time;
extern std::mutex last_input_mutex;
extern std::shared_ptr<gamepad::hook> hook_instance;
extern bool state;

void start_pad_hook();
void end_pad_hook();

}
