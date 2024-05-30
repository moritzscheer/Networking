// Copyright (C) 2024 Moritz Scheer

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "resources.h"
#include "socket.h"
#include "../includes/server.h"
#include "../utils/memory.h"

extern int initialize_resources(Server *server)
{
	int res = allocate_base_memory(server);
	if (res != 0)
	{
		return res;
	}

	res = configure_session(server->ssl);
	if (res != 0)
	{
		return res;
	}

	res = initialize_connection(server->socket, &server->settings);
	if (res != 0)
	{
		return res;
	}

	return initialize_database();
}

extern void cleanup_resources(Server *server)
{
	/* tries to close file descriptor 3 times */
	int count = 0;
	do
	{
		close(server->socket);
		count++;
	}
	while (res < 0 && errno != EBADF && count < 3);

	return free(server);
}
