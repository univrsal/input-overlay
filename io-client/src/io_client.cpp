/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "util.hpp"
#include "network.hpp"
#include "hook.hpp"

int main(int argc, char **argv)
{
	util::config cfg;
	
	if (!network::init())
		return 1;

	if (!util::parse_arguments(argc, argv, &cfg))
		return 2; /* Invalid arguments */

	if (!network::start_connection(&cfg))
	{
		network::close();
		return 3;
	}

	if (!hook::init()) /* This will block if the hook runs */
		return 4;


	return 0;
}