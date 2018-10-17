/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <netlib.h>

namespace network
{
    extern netlib_socket_set sockets;
    extern bool network_state;
    extern bool network_flag;
    
    void start_network();
    
    void close_network();
    
#ifdef WINDOWS
    DWORD WINAPI hook_method(LPVOID arg);
#else
    void* network_thread(void*);
#endif
    
    class remote_connection {
    
    };
}


