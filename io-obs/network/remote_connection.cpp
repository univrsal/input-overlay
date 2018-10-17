/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "remote_connection.hpp"

namespace network
{
    bool network_state = false;
    bool network_flag = true;
    
    void start_network()
    {
        if (network_state)
            return;
        
        if (netlib_init() == 0)
        {
#ifdef WINDOWS
            hook_thread = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(network_thread),
            nullptr, 0, nullptr);
            network_state = hook_thread;
#else
            network_state = pthread_create(&game_pad_hook_thread, nullptr, network_thread, nullptr) == 0;
#endif
        }
    }
    
    void close_network()
    {
    
    }

#ifdef WINDOWS
    DWORD WINAPI network_thread(LPVOID arg);
#else
    void* network_thread(void*)
#endif
    {
        
        while(network_flag)
        {
        
        }

#ifdef WINDOWS
        return 0x0;
#else
        pthread_exit(nullptr);
#endif
    }
}
