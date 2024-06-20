// Copyright (C) 2024, Moritz Scheer

#include <pthread.h>
#include <linux/types.h>
#include <linux/io_uring.h>

#include "worker.h"
#include "threads.h"
#include "read.h"
#include "write.h"
#include "../includes/server.h"

int worker_function()
{
	struct task *task;

	while (1)
	{
		status_code = fetch_message(task);
		if (status_code != 0)
		{
			return status_code;
		}

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

		connection->blocked = false;
	}
}

int *worker_function(struct server *server)
{
	int status_code;            /* local status variable */
	struct msghdr *message;     /* struct containing all packet data */
	int result;                 /* io operation result variable */
	uint8_t event_type, tries;  /* ancillary data saved in msghdr */

	while (1)
	{
		status_code = fetch_message(server->ring, server->cqe, message, &result, &event_type, &tries);
		if (status_code != 0)
		{
			return status_code;
		}

		switch (GET_EVENT_TYPE(message))
		{
			case READ:
				status_code = resolve_read(server->socket, message, result);

				switch (status_code)
				{
					case RESPOND:
						status_code = prepare_write(server->ring, server->socket, message);
						if (status_code != 0)
						{
							return status_code;
						}
						break;
					case DROP:
						status_code = return_buffer(message);
						if (status_code != 0)
						{
							return status_code;
						}
						break;
					default:
						return status_code;
				}

				status_code = prepare_read(server->ring, server->socket);
				if (status_code != 0)
				{
					return status_code;
				}
				break;
			case WRITE:
				status_code = resolve_write(server, message, result);

				switch (status_code)
				{
					case DROP:
						status_code = return_buffer(message);
						if (status_code != 0)
						{
							return status_code;
						}
						break;
					case RETRY:
						status_code = prepare_write(server->ring, server->socket, message);
						if (status_code != 0)
						{
							return status_code;
						}
						break;
					default:
						return status_code;
				}
				break;
			default:
				perror("Unexpected message type");
				break;
		}

		status_code = finish_task();
		if (status_code != 0)
		{
			return status_code;
		}
	}
}
