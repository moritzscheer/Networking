// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "resources.h"
#include "../includes/server.h"
#include "../handlers/io_handler.h"
#include "socket.h"

int initialize_resources(Server *server)
{
	status_code = allocate_base_memory(server);
	if (status_code < 0)
		return status_code;

	status_code = initialize_socket(server->socket);
	if (status_code < 0)
		return status_code;

	status_code = configure_logging();
	if (status_code < 0)
		return status_code;

	status_code = configure_session(server->ssl);
	if (status_code < 0)
		return status_code;

	status_code = initialize_database();
	if (status_code < 0)
		return status_code;

	return 0;
}

void cleanup_resources(Server *server)
{
	/* tries to close file descriptor 3 times */
	int res, count = 0;
	do
	{
		res = close(server->socket);
		count++;
	}
	while (res < 0 && errno != EBADF && count < 3);

	/* free's the io_uring buffer */
	if (server->ring)
		free(server->ring);

	/* free's the main server buffer */
	if (server)
		free(server);
}

static int allocate_base_memory(Server *server)
{
	server = malloc(sizeof(Server));
	if (!server)
	{
		perror("failed to allocate memory for server struct");
		return ENOMEM;
	}
	server->ring = malloc(sizeof(struct io_uring));
	if (!server->ring)
	{
		perror("failed to allocate memory for uring struct");
		return ENOMEM;
	}
	return 0;
}