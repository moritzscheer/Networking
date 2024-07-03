// Copyright (C) 2024, Moritz Scheer

#include <pthread.h>
#include <linux/types.h>
#include <linux/io_uring.h>

#include "worker.h"
#include "threads.h"
#include "read.h"
#include "write.h"
#include "../includes/server.h"
#include "../utils/packet.h"

int *worker_function()
{
	struct pkt *packet;

	while (1)
	{
		status_code = fetch_message(packet);
		if (status_code != 0)
		{
			return status_code;
		}

		handle_quic_events(packet);
		handle_http_events(packet);

		connection->blocked = false;
	}
}

static int handle_quic_events(struct pkt *packet)
{
	ngtcp2_path path;
	memcpy(&path, ngtcp2_conn_get_path(connection->conn), sizeof(path));
	path.remote.addrlen = remote_addrlen;
	path.remote.addr = (struct sockaddr *) &remote_addr;

	ngtcp2_pkt_info pi;
	memset(&pi, 0, sizeof(pi));

	switch (ngtcp2_conn_read_pkt(conn, &path, &pi, buf, n_read, timestamp()))
	{
		case 0:
		{
			return get_response_packet();
		}
		case NGTCP_ERR_RETRY:
		{
			return get_retry_packet();
		}
		case NGTCP_ERR_DROP_CONN:
		{
			return drop_connection();
		}
		case NGTCP_ERR_DRAINING:
		{
			return drain_connection();
		}
		case NGTCP_ERR_CRYPTO:
		{
			return alert_tls_stack();
		}
		default:
		{
			return close_connection();
		}
	}
}

static int handle_http_events(struct pkt *packet)
{

}