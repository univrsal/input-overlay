/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "util.hpp"
#include "network.hpp"
#include "hook.hpp"
#include <stdio.h>

int main(int argc, char **argv)
{
	
	if (!network::init())
		return 1;
	else
	    printf("Network init done.\n");

	if (!util::parse_arguments(argc, argv))
		return 2; /* Invalid arguments */
    else
        printf("Arguments: Port: %hu, Name: %s, IP: %s\n", util::cfg.port, util::cfg.username, argv[1]);

	if (!network::start_connection()) /* Starts a separate network thread */
	{
		network::close();
		return 3;
	}

	if (!hook::init()) /* This will block if the hook runs */
		return 4;

	network::close();
	return 0;
}