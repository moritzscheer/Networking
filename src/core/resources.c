// Copyright (C) 2024 Moritz Scheer

#include "resources.h"
#include "network.h"
#include "threads.h"
#include "../middleware/session.h"
#include "../includes/server.h"
#include "../utils/print.h"
#include "../utils/retry.h"

int initialize_resources(struct server *server)
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

void handle_shutdown(struct server *server)
{
	/* tries to close file descriptor 3 times */
	retry(close, server->socket);

	retry(backup_database, server);

	return free(server);
}

static void set_cpu_affinity(void)
{
	cpu_set_t mask;

	if (cfg_cpu == -1)
	{
		return;
	}

	CPU_ZERO(&mask);
	CPU_SET(cfg_cpu, &mask);
	if (sched_setaffinity(0, sizeof(mask), &mask))
	{
		t_error(1, errno, "unable to pin cpu\n");
	}
}

static void sigint_handler(__attribute__((__unused__)) int sig)
{
	/* kill if should_stop can't unblock threads fast enough */
	if (should_stop)
	{
		_exit(-1);
	}
	should_stop = true;
}

static void set_iowq_affinity(struct io_uring *ring)
{
	cpu_set_t mask;
	int ret;

	if (cfg_cpu == -1)
	{
		return;
	}

	CPU_ZERO(&mask);
	CPU_SET(cfg_cpu, &mask);
	ret = io_uring_register_iowq_aff(ring, 1, &mask);
	if (ret)
	{
		t_error(1, ret, "unabled to set io-wq affinity\n");
	}
}