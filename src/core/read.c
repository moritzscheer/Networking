// Copyright (C) 2024, Moritz Scheer

#include <linux/io_uring.h>
#include <liburing.h>
#include <sys/socket.h>

#include "read.h"
#include "../includes/server.h"
#include "../includes/packet.h"

int listen_loop(struct server *server)
{
	int res, I, count;

	res = setup_context(ring, cqes, msg);
	if (res != 0)
	{
		return res;
	}

	res = prepare_read(ring, server->socket);
	if (res != 0)
	{
		return cleanup(res);
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
			return cleanup(res);
		}

		/* Gets the amount of Completion events from the ring and loops over them. */
		count = io_uring_peek_batch_cqe(ring, &cqes[0], CQES);
		for (I = 0; I < count; I++)
		{
			res = validate_cqe(cqes[I]);
			if (res != 0)
			{
				return cleanup(res);
			}
		}
		io_uring_cq_advance(ring, count);
	}
}

int fetch_read_message(struct pkt *packet)
{
	pthread_mutex_lock(&read_mutex);

	while (IsEmpty(new_conn_queue))
	{
		res = pthread_cond_wait(&cond_fetch, &read_mutex);
		if (res != 0)
		{
			pthread_mutex_unlock(&read_mutex);
			return res;
		}
	}

	Dequeue(new_conn_queue, packet);
	Dequeue(all_conn_queue, packet);

	pthread_mutex_unlock(&read_mutex);
}

static int setup_context(struct io_uring *ring, struct io_uring_cqe *cqe, struct msghdr *msg)
{
	struct io_uring_params params;
	int res;

	memset(&params, 0, sizeof(params));
	params.cq_entries = QD * 8;
	params.flags = IORING_SETUP_SUBMIT_ALL | IORING_SETUP_COOP_TASKRUN | IORING_SETUP_CQSIZE |
	               IORING_SETUP_SINGLE_ISSUER;

	res = io_uring_queue_init_params(QD, ring, &params);
	if (res < 0)
	{
		return res;
	}

	res = setup_buffer_pool();
	if (res)
	{
		io_uring_queue_exit(ring);
	}

	memset(msg, 0, sizeof(msg));
	msg.msg_namelen = sizeof(struct sockaddr_storage);
	msg.msg_controllen = CONTROLLEN;

	res = io_uring_register_files(&ring, &sockfd, 1);
	if (res)
	{
		return res;
	}

	return res;
}

static int cleanup(int result)
{
	return result;
}

static int prepare_read(io_uring *ring, int socket, struct msghdr *msg)
{
	struct io_uring_sqe *sqe;
	if (get_sqe(ring, &sqe) == -1)
	{
		return -1;
	}

	io_uring_prep_recvmsg_multishot(sqe, socket, msg, MSG_TRUNC);
	sqe->flags |= IOSQE_FIXED_FILE;

	sqe->flags |= IOSQE_BUFFER_SELECT;
	sqe->buf_group = 0;

	io_uring_sqe_set_data64(sqe, NULL);
	return 0;
}

static inline int get_sqe(struct io_uring *ring, struct io_uring_sqe **sqe)
{
	*sqe = io_uring_get_sqe(ring);

	if (!*sqe)
	{
		io_uring_submit(ring);
		*sqe = io_uring_get_sqe(ring);
	}
	if (!*sqe)
	{
		return -1;
	}
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

	/* If operation was not successful */
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

		return resolve_success(cqe->res);
	}
	return resolve_error(server, msg, cqe->res);
}

static inline int resolve_success(size_t buffer_length)
{
	ngtcp2_version_cid *cid;

	struct pkt *packet = (struct pkt *)
		{
			.iov_base = io_uring_recvmsg_payload(msg_out, msg),
			.iov_len = io_uring_recvmsg_payload_length(msg_out, buffer_length, msg)
		}

	/* decodes packet header and associates a connection to it */
	switch (ngtcp2_pkt_decode_version_cid(cid, packet->iov_base, packet->iov_len, NGTCP2_MAX_CIDLEN))
	{
		case 0:
		{
			pthread_mutex_lock(&read_mutex);

			packet->connection = find_connection(cid);
			if (!packet->connection)
			{
				packet->connection = create_connection();
				if (!packet->connection)
				{
					pthread_mutex_unlock(&read_mutex);
					return NOMEM;
				}

				if (IsEmpty(new_conn_queue))
				{
					int res = send_signal(&cond_fetch);
					if (res != 0)
					{
						return res;
					}
				}

				Enqueue(new_conn_queue, packet);
			}
			Enqueue(all_conn_queue, packet);

			pthread_mutex_unlock(&read_mutex);
			return;
		}
		case NGTCP2_ERR_VERSION_NEGOTIATION:
		{
			return write_version_negotiation_packet(cid, ep, local_addr, sa, salen);
		}
		default:
		{
			return;
		}
	}
}

static inline int resolve_error(struct server *server, struct msghdr *msg, int result)
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
			return prepare_read(ring, server->socket);
		}
		case EINTR: // The call was interrupted by a signal before any data was received.
		{
			return handle_interrupted_call();
		}
		case ECONNRESET: // Connection reset by peer.
		{
			return handle_drop_connection();
		}
		case ETIMEDOUT: // The connection timed out during the receive operation.
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