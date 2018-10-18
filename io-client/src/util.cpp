/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */
#include "util.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>

namespace util
{
	 bool parse_arguments(int argc, char ** args)
	 {
		 cfg.monitor_gamepad = false;
		 cfg.monitor_keyboard = true;
		 cfg.monitor_mouse = false;
		 cfg.port = 1608;
		 
         if (argc < 2)
         {
			 printf("io_client usage:\n [ip] {port} {other options}\n");
			 printf(" [] => required {} => optional\n");
			 printf(" [ip]          can be ipv4 or hostname\n");
			 printf(" {port}        default is 1608\n");
			 printf(" --gamepad=1   enable/disable gamepad monitoring. Off by default\n");
			 printf(" --mouse=1     enable/disable mouse monitoring.  Off by default\n");
			 printf(" --keyboard=1  enable/disable keyboard monitoring. On by default\n");
			 return false;
         }

	     auto newport = uint16_t(strtol(args[2], nullptr, 0));
	     if (newport > 1024) /* No system ports pls */
	     {
	         cfg.port = newport;
	     }
	     else
	     {
	         printf("%hu is outside the valid port range [1024 - %i]", newport, 0xffff);
	     }

		 std::string arg;
         for (auto i = 3; i < argc; i++)
         {
             arg = args[i];
             if (arg.find("--gamepad") != std::string::npos)
                 cfg.monitor_gamepad = arg.find('1') != std::string::npos;
             else if (arg.find("--mouse") != std::string::npos)
                 cfg.monitor_mouse = arg.find('1') != std::string::npos;
             else if (arg.find("--keyboard") != std::string::npos)
                 cfg.monitor_keyboard = arg.find('1') != std::string::npos;
         }
	 }
 }
