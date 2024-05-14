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

	status_code = initialize_resources(server);
	if (status_code < 0)
	{
		cleanup_resources(server);
		return status_code;
	}

	status_code = server_loop(server);

	cleanup_resources(server);

	return status_code < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

