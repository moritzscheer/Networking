// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "./core/resources.h"
#include "./core/loop.h"
#include "./includes/server.h"
#include "includes/errno2.h"

int main()
{
	server *server;
	res = EXIT_SUCCESS;

	res = initialize_resources(server);
	if (res < 0)
	{
		cleanup_resources(server);
		return res;
	}

	res = server_loop(server);
	cleanup_resources(server);
	return res;
}