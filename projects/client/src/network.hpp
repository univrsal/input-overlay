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
#include <netlib.h>
#include <thread>
#include <buffer.hpp>
#include <mutex>
#include <atomic>

namespace network {
extern tcp_socket sock;
extern netlib_socket_set set;
extern bool connected;
extern bool state;
extern std::atomic<bool> network_loop;
/* Set to true by other threads */
/* Set to true to prevent other threads from modifying data, which is about to be sent */
extern buffer buf;
extern std::mutex buffer_mutex;
extern std::thread network_thread;

bool init();
bool start_connection();
void start_thread();
bool listen();

void network_thread_method();

void close();
}
