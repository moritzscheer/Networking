// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "read.h"
#include "../handlers/parser.h"
#include "../handlers/buffer.h"
#include "../includes/message.h"
#include "../includes/status.h"
#include "../includes/server.h"

int prepare_read(io_uring *ring, int socket, struct msghdr *message)
{
	if (message == NULL)
	{
		int res = get_buffer(message);
		if (res == NO_FREE_BUF)
		{
			return res;
		}
	}

	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
	if (sqe == NULL)
	{
		return SQ_FULL;
	}

	set_event_type(message, READ);

	io_uring_prep_recvmsg(sqe, socket, message, 0);
	io_uring_sqe_set_data(sqe, message);
	return 0;
}

int resolve_read(server *server, message *message, int result)
{
	if (result >= 0)
	{
		int res = parse_read(server, message);

		switch (res)
		{
			case END_OF_STREAM:
			{
				close_stream(message);
				return RESPOND;
			}
			case END_OF_PACKET:
			{
				return RESPOND;
			}
			default:
			{
				return DROP;
			}
		}
	}
	else
	{
		switch (result)
		{
			case: -EAGAIN || -EWOULDBLOCK
				return RETRY;
			case: -EBADF
				return RETRY;
			case: -EFAULT
				return RETRY;
			case: -EINVAL
				return RETRY;
			case: -EIO
				return RETRY;
			case: -ENOSPC
				return RETRY;
			case: -ENOMEM
				return DROP;
			default:
				return DROP;
		}
	}
}

int parse_read(Server *server, struct msghdr *msghdr)
{
	for (int I = 0; I < msghdr->msg_iovlen; I++)
	{
		uint32_t version;
		const uint8_t *scid, *dcid;
		size_t dcidlen, scidlen;

		switch (ngtcp2_pkt_decode_version_cid(&vc, data.data(), data.size(), NGTCP2_SV_SCIDLEN))
		{
			case 0:
			{
				break;
			}
			case NGTCP2_ERR_VERSION_NEGOTIATION:
			{
				send_version_negotiation(vc.version, {vc.scid, vc.scidlen}, {vc.dcid, vc.dcidlen}, ep, local_addr, sa,
				                         salen);
				return;
			}
			default:
			{
				return;
			}
		}







		Connection *connection = find_connection(server->connections, dcid, dcidlen);
		if (!connection)
		{
			connection = create_connection();
			if (!connection)
			{
				return -1;
			}
		}

		ngtcp2_path path;
		memcpy(&path, ngtcp2_conn_get_path(connection->conn), sizeof(path));
		path.remote.addrlen = remote_addrlen;
		path.remote.addr = (struct sockaddr*) &remote_addr;

		ngtcp2_pkt_info pi;
		memset(&pi, 0, sizeof(pi));

		ret = ngtcp2_conn_read_pkt (conn, &path, &pi, buf, n_read, timestamp ());
		if (ret == NGTCP_ERR_RETRY)
		{

		}
		else if (ret == NGTCP_ERR_DROP_CONN)
		{
			HASH_DEL(server->connections, connection);
			free_connection(connection);
		}
		else if (ret == NGTCP_ERR_DRAINING)
		{

		}
		else if (ret == NGTCP_ERR_CRYPTO)
		{

		}
		else
		{
			ngtcp2_conn_write_connection_close();
		}
	}
	return 0;
}

static int decode(struct iovec iov, uint32_t *version, const uint8_t **dcid, size_t *dcidlen, const uint8_t **scid, size_t *scidlen)
{
	int ret = ngtcp2_pkt_decode_version_cid(version,
	                                        dcid, dcidlen,
	                                        scid, scidlen,
	                                        iov.iov_base, iov.iov_len,
	                                        NGTCP2_MAX_CIDLEN);
	if (ret == NGTCP2_ERR_INVALID_ARGUMENT)
	{
		return -1;
	}
	else if (ret == NGTCP2_ERR_VERSION_NEGOTIATION)
	{
		return ngtcp2_pkt_write_version_negotiation();
	}
	return 0;
}