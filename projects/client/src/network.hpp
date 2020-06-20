/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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
#include "util.hpp"

#define BUFFER_SIZE 512
#define LISTEN_TIMEOUT 25

namespace network {
extern tcp_socket sock;
extern netlib_socket_set set;
extern bool connected;
extern bool state;
extern volatile bool network_loop;
extern volatile bool need_refresh; /* Set to true by other threads */
extern volatile bool data_block; /* Set to true to prevent other threads from modifying data, which is about to be sent */
extern netlib_byte_buf *buffer; /* Shared buffer for writing data, which will be sent to the server */
extern std::thread network_thread;

bool init();
bool start_connection();
void start_thread();
bool listen();

void network_thread_method();

void close();
}
