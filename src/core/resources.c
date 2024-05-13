// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <string.h>

#include "resources.h"
#include "../handlers/io_handler.h"
#include "socket.h"

int init_resources(Server *server)
{
	server = malloc(sizeof(Server));
	if (!server)
	{
		return ENOMEM;
	}

	status_code = init_socket(server->socket);
	if (status_code < 0)
	{
		return status_code;
	}

	status_code = init_uring(server);
	if (status_code < 0)
	{
		return status_code;
	}

	/*
	result = init_threads(server->threads);
	if (result < 0)
	{

	}
	*/

	status_code = init_database();
	if (status_code < 0)
	{

	}

	return 0;
}

void cleanup_resources(Server *server)
{
	if(server->ring)
	{
		free(server->ring)
	}
	if(server)
	{
		free(server);
	}
}
