// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "./core/resources.h"
#include "./core/loop.h"
#include "./includes/server.h"

static int status_code;

int main()
{
	Server *server;

	status_code = init_resources(server);
	if (status_code < 0)
	{
		cleanup_resources(server);
		return status_code;
	}

	status_code = configure_logging(server);
	if (status_code < 0)
	{
		cleanup_resources(server);
		return status_code;
	}

	status_code = server_loop(server->ring, server->cqe, server->socket);
	if (status_code < 0)
	{
		cleanup_resources(server);
		return status_code;
	}

	cleanup_resources(server);
	return EXIT_SUCCESS;
}

