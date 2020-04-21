/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

#include "messages.hpp"
#include <netlib.h>

#define TIMEOUT_NS (1000 * 1000 * 1000)
namespace network {
class io_server;

extern bool network_state; /* Initialization state*/
extern bool network_flag;  /* Running state */
/* Set in obs_module_load */
extern bool local_input;
extern char local_ip[16];

const char *get_status();

void start_network(uint16_t port);

void close_network();

void *network_handler(void *);

char *read_text(tcp_socket sock, char **buf);

message read_msg_from_buffer(netlib_byte_buf *buf);

int send_message(tcp_socket sock, message msg);

extern io_server *server_instance;
}
