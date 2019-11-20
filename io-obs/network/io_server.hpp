/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2019 univrsal <universailp@web.de>.
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

#include "io_client.hpp"
#include <netlib.h>
#include <vector>
#include <memory>
#include <obs-module.h>
#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#endif

#define BUFFER_SIZE 90
#define LISTEN_TIMEOUT 25
enum message;

namespace network
{
    extern std::mutex mutex;

    class io_server
    {
    public:
        io_server(uint16_t port);

        ~io_server();

        bool init();

        void listen(int &numready);

        tcp_socket socket() const;

        void add_client(tcp_socket socket, char* name);

        void update_clients();

        void get_clients(std::vector<const char*> &v);

        void get_clients(obs_property_t* prop, bool enable_local);

        bool clients_changed() const;

        void ping_clients();

        /* Checks clients and removes them
         * if necessary
         */
        void roundtrip();

        io_client* get_client(uint8_t id);

    private:
        bool unique_name(char* name);

        static void fix_name(char* name);

        bool create_sockets();

        uint64_t m_last_refresh = 0;
        netlib_byte_buf* m_buffer = nullptr; /* Used for temporarily storing sent data */
        bool m_clients_changed = false; /* Set to true on connection/disconnect and false after get_clients() */
        uint8_t m_num_clients;
        ip_address m_ip{};
        tcp_socket m_server;
        std::vector<std::unique_ptr<io_client>> m_clients;
    };
}


