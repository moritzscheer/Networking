// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "./core/resources.h"
#include "./core/loop.h"
#include "./includes/server.h"
#include "./utils/print.h"

int main()
{
	Server *server = NULL;

	int res = initialize_resources(server);
	if (res < 0)
	{
		return print_status_code(res);
	}

	res = server_loop(server);
	
	cleanup_resources(server);

	return print_status_code(res);
}