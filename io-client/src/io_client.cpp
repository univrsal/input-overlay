#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <stdlib.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <string.h>
#include "SDL_net.h"

int main(int argc, char **argv)
{
	IPaddress ip;
	TCPsocket sock;
	char message[1024];
	int len;
	Uint16 port;

	/* check our commandline */
	if (argc<3)
	{
		printf("%s host port\n", argv[0]);
		exit(0);
	}

	/* initialize SDL_net */
	if (SDLNet_Init() == -1)
	{
		printf("SDLNet_Init: %s\n", SDLNet_GetError());
		exit(2);
	}

	/* get the port from the commandline */
	port = (Uint16)strtol(argv[2], NULL, 0);

	/* Resolve the argument into an IPaddress type */
	if (SDLNet_ResolveHost(&ip, argv[1], port) == -1)
	{
		printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(3);
	}

	/* open the server socket */
	sock = SDLNet_TCP_Open(&ip);
	if (!sock)
	{
		printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(4);
	}

	/* read the buffer from stdin */
	printf("Enter Message, or Q to make the server quit:\n");
    while (strcmp(message, "Q") != 0)
    {
		fgets(message, 1024, stdin);
		len = strlen(message);

		/* strip the newline */
		message[len - 1] = '\0';

		if (len)
		{
			int result;

			/* print out the message */
			printf("Sending: %.*s\n", len, message);

			result = SDLNet_TCP_Send(sock, message, len); /* add 1 for the NULL */
			if (result<len)
				printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		}
    }
	

	SDLNet_TCP_Close(sock);

	/* shutdown SDL_net */
	SDLNet_Quit();

	return(0);
}