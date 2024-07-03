// Copyright (C) 2024, Moritz Scheer

#include <linux/io_uring.h>
#include <liburing.h>
#include <sys/socket.h>
#include <string.h>

#include "read.h"
#include "../middleware/io_uring.h"
#include "threads.h"
#include "../includes/server.h"
#include "../utils/packet.h"

int listen_loop(struct server *server)
{
	int res, I, completions;

	res = setup_ring(server->socket);
	if (res != 0)
	{
		return res;
	}

	while (1)
	{
		res = io_uring_submit_and_wait(ring, 1);
		if (res == -EINTR)
		{
			continue;
		}
		else if (res < 0)
		{
			return cleanup_ring(res);
		}

		completions = io_uring_peek_batch_cqe(ring, &cqes[0], CQES);
		for (I = 0; I < completions; I++)
		{
			res = validate_cqe(cqes[I]);
			if (res != 0)
			{
				return cleanup_ring(res);
			}
		}
		io_uring_cq_advance(ring, completions);
	}
}

static int prepare_sqe(int socket)
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
	if (!sqe)
	{
		io_uring_submit(ring);
		sqe = io_uring_get_sqe(ring);
		if (!sqe)
		{
			return -1;
		}
	}

	io_uring_prep_recvmsg_multishot(sqe, socket, msg, MSG_TRUNC);
	sqe->flags |= IOSQE_FIXED_FILE;

	sqe->flags |= IOSQE_BUFFER_SELECT;
	sqe->buf_group = 0;

	io_uring_sqe_set_data64(sqe, NULL);
	return 0;
}

static inline int validate_cqe(struct io_uring_cqe *cqe)
{
	if (!(cqe->flags & IORING_CQE_F_MORE))
	{
		res = prepare_read(ring, server->socket, msg);
		if (res != 0)
		{
			return res;
		}
	}

	/* If operation was successful */
	if (cqe->res >= 0 && (cqe->flags & IORING_CQE_F_BUFFER))
	{
		idx = cqe->flags >> 16;

		msg_out = io_uring_recvmsg_validate(get_buffer(cqe->flags >> 16), cqe->res, &msg);
		if (!msg_out)
		{
			return 0;
		}

		if (msg_out->flags & MSG_TRUNC)
		{
			return -1;
		}

		if (msg_out->namelen > msg.msg_namelen)
		{
			recycle_buffer(ctx, idx);
			return -1;
		}

		return resolve_success();
	}
	return resolve_error(server, msg, cqe->res);
}

static inline int resolve_success()
{
	ngtcp2_version_cid vcid;

	switch (ngtcp2_pkt_decode_version_cid(&vcid, packet->iov_base, packet->iov_len, NGTCP2_MAX_CIDLEN))
	{
		case 0:
		{
			struct pkt *packet = create_packet(msg_out, prev_pkt, vcid);

			struct connection connection = find_connection(packet->dcid);
			if (!connection)
			{
				connection = create_connection(cid, sockaddr, sizeof(sockaddr));
				if (!connection)
				{
					return NOMEM;
				}
			}

			prev_pkt = packet;
			enqueue_packet(pkt);
			return 0;
		}
		case NGTCP2_ERR_VERSION_NEGOTIATION:
		{
			return write_version_negotiation_packet(cid, ep, local_addr, sa, salen);
		}
		default:
		{
			return -1;
		}
	}
}

static inline int resolve_error(int socket, int result)
{
	switch (result)
	{
		case -ENOBUFS:
		{
			return 0;
		}
		case EAGAIN: // The socket is marked non-blocking and no data is available to be read.
		case EWOULDBLOCK:
		{
			return prepare_read(ring, socket);
		}
		case EINTR: // The call was interrupted by a signal before any data was received.
		{
			return handle_interrupted_call();
		}
		case ECONNRESET: // Connection reset by peer.
		{
			return handle_drop_connection();
		}
		case ETIMEDOUT: // The connection timed out during the read operation.
		{
			return handle_wait_and_retry();
		}
		case ENOTCONN: // The socket is not connected.
		case ENETUNREACH: // The network is unreachable.
		case EHOSTUNREACH: // The host is unreachable.
		case ECONNREFUSED: // A remote host refused to allow the network connection.
		{
			return handle_wait_and_retry();
		}
		case ENETDOWN: // The network is down.
		case ENETRESET: // The network dropped the connection on reset.
		{
			return handle_reconnect();
		}
		case EBADF: // The socket argument is not a valid file descriptor.
		case EINVAL: // Invalid argument passed.
		case ENOMEM: // No memory available to complete the operation.
		case ENOTSOCK: // The file descriptor does not refer to a socket.
		case EOPNOTSUPP: // Some operation is not supported on the socket.
		case EFAULT: // The msg argument points outside the accessible address space.
		{
			return handle_abort_operation();
		}
		case EIO: // An I/O error occurred.
		case ENOMSG: // No message of the desired type.
		case EPROTO: // Protocol error.
		default:
		{
			return handle_unknown_error();
		}
	}
}