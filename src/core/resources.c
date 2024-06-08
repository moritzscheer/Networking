// Copyright (C) 2024 Moritz Scheer

#include "resources.h"
#include "network.h"
#include "threads.h"
#include "../handlers/session.h"
#include "../includes/server.h"
#include "../utils/print.h"
#include "../utils/retry.h"

int initialize_resources(Server *server)
{
	int res;

	/*
	 *
	 */
	start_step("Allocate base memory");
	res = create_buffer_pool();
	if (res != 0)
	{
		return end_step("Failed to allocate base memory", res);
	}
	end_step("Base memory allocated", res);

	/*
	 *
	 */
	start_step("Establish server connection");
	res = initialize_connection(server);
	if (res != 0)
	{
		return end_step("Failed to establish server connection", res);
	}
	end_step("Server connection established", res);

	/*
	 *
	 */
	start_step("Configure session");
	res = initialize_session(server);
	if (res != 0)
	{
		return end_step("Failed to configure session", res);
	}
	end_step("Session configured", res);

	/*
	 *
	 */
	start_step("Configure Threads");
	res = initialize_threads(server);
	if (res != 0)
	{
		return end_step("Failed to configure threads", res);
	}
	end_step("Threads configured", res);

	/*
	 *
	 */
	start_step("Configure database");
	res = initialize_database(server);
	if (res != 0)
	{
		return end_step("Failed to configure database", res);
	}
	end_step("Database configured", res);

	return res;
}

void handle_shutdown(Server *server)
{
	/* tries to close file descriptor 3 times */
	retry(close, server->socket);

	retry(backup_database, server);

	return free(server);
}