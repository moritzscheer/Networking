// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <errno.h>

#include "../handlers/buffer.h"
#include "../includes/server.h"
#include "../includes/server.h"
#include "print.h"

void *allocate_memory(size_t num, size_t size)
{
	*ptr = calloc(num, size);
	if (ptr == NULL)
	{
		perror("Memory allocation failed");
	}
	return ptr;
}

int allocate_base_memory(Server *server)
{
	start_step("Allocate base memory");

	server = (Server *) calloc(1, sizeof(Server));
	if (!server)
	{
		return end_step("Failed to allocate base memory", ENOMEM);
	}

	int res = create_buffer_pool();
	if (res != 0)
	{
		free(server);
		return end_step("Failed to allocate base memory", ENOMEM);
	}

	return end_step("Base memory allocated", 0);
}