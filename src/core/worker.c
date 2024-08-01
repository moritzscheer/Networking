// Copyright (C) 2024, Moritz Scheer

#include <string.h>
#include <ngtcp2/ngtcp2.h>

#include "worker.h"
#include "../includes/server.h"
#include "../handlers/connection.h"
#include "../handlers/read/read_queue.h"

int *worker_function()
{
	int res;
	struct connection *connection;
	struct rqe *entry;

	while (1)
	{
		res = dequeue_rqe(connection, entry);
		if (res != 0)
		{
			break;
		}

		res = handle_quic_events(connection, task);
		if (res != 0)
		{
			break;
		}

		if(connection->nghttp3_conn)
		{
			res = handle_http_events(pkt);
			if (res != 0)
			{
				break;
			}
		}

		finish_rqe(connection, entry);
	}
	return res;
}


static void handle_quic_events(struct connection *connection, struct rqe *entry)
{
	ngtcp2_path path;
	memcpy(&path, ngtcp2_conn_get_path(connection->ngtcp2_conn), sizeof(path));
	path.remote.addrlen = remote_addrlen;
	path.remote.addr = (struct sockaddr *) &remote_addr;

	ngtcp2_pkt_info pi;
	memset(&pi, 0, sizeof(pi));

	switch (ngtcp2_conn_read_pkt(connection->ngtcp2_conn, &path, &packet_info, entry->iov_base, entry->iov_len,
								 entry->timestamp))
	{
		case 0:
		{
			break;
		}
		case NGTCP_ERR_RETRY:
		{
			return send_retry_packet(read_pkt->version, connection->cid, read_pkt->scid, , );
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
			return close_connection(read_pkt->version, connection->cid, read_pkt->scid, , );
		}
	}
}

static void handle_http_events(struct read_pkt *pkt)
{
	nghttp3_conn *conn, int64_t pkt, const uint8_t *src, size_t srclen, int fin
	nghttp3_ssize cons_bytes = nghttp3_conn_read_stream(pkt->nghttp3_conn);
	if (cons_bytes < 0)
	{
		nghttp3_conn_del(pkt->nghttp3_conn);
	}
	return 0;
}