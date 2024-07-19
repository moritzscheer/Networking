// Copyright (C) 2024, Moritz Scheer

#include <string.h>
#include <ngtcp2/ngtcp2.h>

#include "worker.h"
#include "../includes/server.h"
#include "../handlers/connection.h"

int *worker_function()
{
	int res;
	struct connection *connection;
	struct task *task;

	while (1)
	{
		res = dequeue_connection(connection, task);
		if (res != 0)
		{
			return res;
		}

		res = handle_quic_events(connection, task);
		if (res != 0)
		{
			return res;
		}

		if(connection->nghttp3_conn)
		{
			res = handle_http_events(packet);
			if (res != 0)
			{
				return res;
			}
		}

		connection->blocked = false;
	}
}


static void handle_quic_events(struct connection *connection, struct pkt *pkt)
{
	ngtcp2_path path;
	memcpy(&path, ngtcp2_conn_get_path(connection->ngtcp2_conn), sizeof(path));
	path.remote.addrlen = remote_addrlen;
	path.remote.addr = (struct sockaddr *) &remote_addr;

	ngtcp2_pkt_info pi;
	memset(&pi, 0, sizeof(pi));

	switch (ngtcp2_conn_read_pkt(connection->ngtcp2_conn, &path, &packet_info, iov_base, iov_len, timestamp()))
	{
		case 0:
		{

		}
		case NGTCP_ERR_RETRY:
		{
			return send_retry_packet(pkt->version, connection->cid, pkt->scid, , );
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
			return close_connection(pkt->version, connection->cid, pkt->scid, , );
		}
	}
}

static void handle_http_events(struct pkt *packet)
{
	nghttp3_conn *conn, int64_t packet, const uint8_t *src, size_t srclen, int fin
	nghttp3_ssize cons_bytes = nghttp3_conn_read_stream(packet->nghttp3_conn);
	if (cons_bytes < 0)
	{
		nghttp3_conn_del(packet->nghttp3_conn);
	}
	return 0;
}