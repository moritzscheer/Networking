// Copyright (C) 2024 Moritz Scheer

#include "./core/resources.h"
#include "./core/worker.h"
#include "./includes/server.h"
#include "./utils/print.h"

int main(void)
{
	struct server *server = {0};

	int status_code = initialize_resources(server);
	if (status_code == 0)
	{
		status_code = server_loop(server);
	}

	handle_shutdown(server);
	return print_status_code(status_code);
}