/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <netlib.h>
#include "util.hpp"

namespace network
{
	extern tcp_socket sock;
	extern netlib_socket_set set;

	bool init();
	bool start_connection(util::config* cfg);

	void close();
}