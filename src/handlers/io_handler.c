// Copyright (C) 2024, Moritz Scheer

#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io_handler.h"
#include "../includes/server.h"
#include "../utils/io_operations.h"
#include "../includes/IORequest.h"

int init_uring(Server *server)
{
	server->ring = malloc(sizeof(struct io_uring));
	if (!server->ring)
	{
		return ENOMEM;
	}

	if (io_uring_queue_init(QUEUE_DEPTH, server->ring, 0) < 0)
	{
		perror("Could not initialize io_uring queue.");
		return errno;
	}
	prep_read_io(server->ring, server->socket);
	return 0;
}