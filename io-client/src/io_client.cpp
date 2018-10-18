/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "util.hpp"


int main(int argc, char **argv)
{
	if (!util::parse_arguments(argc, argv))
		return 0; /* Invalid arguments */

	return 0;
}