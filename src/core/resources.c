// Copyright (C) 2024 Moritz Scheer

#include "resources.h"
#include "loop.h"
#include "network.h"
#include "threads.h"
#include "../middleware/session.h"
#include "../middleware/quic/ngtcp2.h"
#include "../middleware/http3/nghttp3.h"
#include "../includes/server.h"
#include "../includes/status.h"
#include "../utils/print.h"

int initialize_resources()
{
	print_step("Initialize the server connection.",
			   "Failed to initialize server connection.",
			   "Server connection initialized.",
			   setup_listening_socket);

	print_step("Initialize threads.",
	           "Failed to initialize threads.",
	           "Threads initialized.",
	           initialize_threads);

	print_step("Initialize database.",
	           "Failed to initialize database.",
	           "Database initialized.",
	           initialize_database);

	print_step("Initialize session.",
	           "Failed to initialize session.",
	           "Session initialized.",
	           initialize_session);

	print_step("Configure ngtcp2 library.",
	           "Failed to configure ngtcp2 library.",
	           "Ngtcp2 library configured.",
	           setup_ngtcp2);

	print_step("Configure nghttp3 library.",
	           "Failed to configure nghttp3 library.",
	           "Nghttp3 library configured.",
	           setup_nghttp3);

	return res;
}

void handle_shutdown()
{
	switch (status_code)
	{
		case INVALID_ARG:
			break;
		case GET_SQE_ERR:
			break;
		case GET_MSG_OUT_ERR:
			break;
		case MSG_TRUNC_ERR:
			break;
		case THREAD_ERR:
			break;
		default:
			strerror(status_code);
			break;
	}

	close(server->socket)
	backup_database();
	free(server)

	return status_code;
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

	if (cfg_cpu == -1)
	{
		return;
	}

	CPU_ZERO(&mask);
	CPU_SET(cfg_cpu, &mask);
	res = io_uring_register_iowq_aff(ring, 1, &mask);
	if (res)
	{
		t_error(1, res, "unabled to set io-wq affinity\n");
	}
}