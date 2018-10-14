#define SDL_MAIN_HANDLED

#include <stdlib.h>
#ifndef _MSC_VER
#include <unistd.h>
#define _sleep usleep
#endif
#include "SDL_net.h"
#include <stdio.h>
#include <cstring>

int32_t swap_int32(int32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | ((val >> 16) & 0xFFFF);
}

int main(int argc, char **argv)
{
	IPaddress ip, *remoteip;
	TCPsocket server, client;
	char message[1024];
	int len;
	Uint32 ipaddr;
	Uint16 port;

	/* check our commandline */
	if (argc<2)
	{
		printf("%s port\n", argv[0]);
		exit(0);
	}

	/* initialize SDL_net */
	if (SDLNet_Init() == -1)
	{
		printf("SDLNet_Init: %s\n", SDLNet_GetError());
		exit(2);
	}

	/* get the port from the commandline */
	port = (Uint16)strtol(argv[1], NULL, 0);

	/* Resolve the argument into an IPaddress type */
	if (SDLNet_ResolveHost(&ip, NULL, port) == -1)
	{
		printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(3);
	}

	/* open the server socket */
	server = SDLNet_TCP_Open(&ip);
	printf("HEYY\n");
	if (!server)
	{
		printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(4);
	}

	while (1)
	{
		/* try to accept a connection */

		client = SDLNet_TCP_Accept(server);
		if (!client)
		{ /* no connection accepted */
		    //printf("SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
			_sleep(500);
			continue;
		}

		/* get the clients IP and port number */
		remoteip = SDLNet_TCP_GetPeerAddress(client);
		if (!remoteip)
		{
			printf("SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());
			continue;
		}

		/* print out the clients IP and port number */
		ipaddr = swap_int32(remoteip->host);
		printf("Accepted a connection from %d.%d.%d.%d port %hu\n",
			ipaddr >> 24,
			(ipaddr >> 16) & 0xff,
			(ipaddr >> 8) & 0xff,
			ipaddr & 0xff,
			remoteip->port);
        while (strcmp(message, "Q") != 0)
        {
			/* read the buffer from client */
			len = SDLNet_TCP_Recv(client, message, 1024);

			if (!len)
			{
				printf("SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
				continue;
			}

			/* print out the message */
			printf("Received: %.*s\n", len, message);
        }
		SDLNet_TCP_Close(client);
        
		if (message[0] == 'Q')
		{
			printf("Quitting on a Q received\n");
			break;
		}
	}

	/* shutdown SDL_net */
	SDLNet_Quit();

	return(0);
}
