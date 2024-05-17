// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "resources.h"
#include "../includes/server.h"
#include "socket.h"
#include "../includes/errno2.h"

int initialize_resources(server *server)
{
	printf("-----------------------------------------------\n"
	       "      Project configuration steps [0/6]        \n"
	       "-----------------------------------------------\n\n")

	res = allocate_base_memory(server);
	if (res < 0)
		return res;
	printf("[1/6] Base memory allocated\n");

	res = initialize_socket(server->socket);
	if (res < 0)
		return res;
	printf("[2/6] Socket configured\n");

	res = initialize_database();
	if (res < 0)
		return res;
	printf("[3/6] Database configured\n")

	res = configure_logging();
	if (res < 0)
		return res;
	printf("[4/6] Logging configured\n");

	res = configure_session(server->ssl);
	if (res < 0)
		return res;
	printf("[5/6] SSL configured\n");

	res = configure_ngtcp2(&server.settings);
	if (res < 0)
		return res;
	printf("[6/6] ngtcp2 configured\n");

	printf("\n"
	       "-----------------------------------------------\n"
	       "             Project is configured             \n"
	       "-----------------------------------------------\n"
	       "\n");
	return 0;
}

void cleanup_resources(server *server)
{
	/* tries to close file descriptor 3 times */
	int res, count = 0;
	do
	{
		res = close(server->socket);
		count++;
	}
	while (res < 0 && errno != EBADF && count < 3);

	/* free's the main server buffer */
	if (server)
		free(server);
}

static int allocate_base_memory(server *server, uring *uring)
{
	server = malloc(sizeof(struct server));
	if (!server)
	{
		perror("failed to allocate memory for server struct");
		return ENOMEM;
	}
	return 0;
}