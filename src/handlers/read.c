// Copyright (C) 2024, Moritz Scheer

#include <linux/io_uring.h>
#include <liburing.h>
#include <sys/socket.h>
#include <string.h>
#include <ngtcp2/ngtcp2.h>

#include "read.h"
#include "connection.h"
#include "../includes/ring.h"
#include "../includes/server.h"
#include "../includes/status.h"

int prepare_read()
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	if (!sqe)
	{
		io_uring_submit(&ring);
		sqe = io_uring_get_sqe(&ring);
		if (!sqe)
		{
			return GET_SQE_ERR;
		}
	}

	io_uring_prep_recvmsg_multishot(sqe, socket, &msg, MSG_TRUNC);
	sqe->flags |= IOSQE_FIXED_FILE;

	sqe->flags |= IOSQE_BUFFER_SELECT;
	sqe->buf_group = 0;

	io_uring_sqe_set_data(sqe, READ);
	return 0;
}

inline int validate_read(struct io_uring_cqe *cqe)
{
	if (cqe->res >= 0 && (cqe->flags & IORING_CQE_F_BUFFER))
	{
		idx = cqe->flags >> 16;

		msg_out = io_uring_recvmsg_validate(get_buffer(cqe->flags >> 16), cqe->res, &msg);
		if (!msg_out)
		{
			return GET_MSG_OUT_ERR;
		}

		if (msg_out->flags & MSG_TRUNC)
		{
			return MSG_TRUNC_ERR;
		}

		if (msg_out->namelen > msg.msg_namelen)
		{
			recycle_buffer(ctx, idx);
			return -1;
		}

		return resolve_success(io_uring_recvmsg_payload(msg_out), io_uring_recvmsg_payload_length(&msg));
	}
	return resolve_error(cqe->res);
}

static inline int resolve_success(void *iov_base, size_t iov_len)
{
	ngtcp2_version_cid vcid;

	switch ((res = ngtcp2_pkt_decode_version_cid(&vcid, iov_base, iov_len, NGTCP2_MAX_CIDLEN)))
	{
		case 0:
		{
			ngtcp2_cid dcid;
			memset(&dcid, 0, sizeof(ngtcp2_cid));
			ngtcp2_cid_init(&dcid, vcid->dcid, vcid->dcidlen);

			struct connection *connection = find_connection(dcid);
			if (!connection)
			{
				connection = create_connection(dcid);
				if (!connection)
				{
					return NOMEM;
				}
			}

			struct read_pkt *pkt = calloc(1, sizeof(struct read_pkt));
			if (pkt)
			{
				ngtcp2_cid scid;
				memset(&scid, 0, sizeof(ngtcp2_cid));
				ngtcp2_cid_init(&scid, vcid->dcid, vcid->scidlen);

				*pkt = (struct read_pkt) {
					.version = vcid->version,
					.scid = scid,
					.receive_time = timestamp_ns(),
					.iov_base = iov_base,
					.iov_len = iov_len,
					.prev_recv_pkt = prev_pkt
				};

				if (!connection->head)
				{
					connection->head = pkt;
					connection->tail = pkt;
				}
				else
				{
					connection->tail->next = pkt;
					connection->tail = tail->next;
				}

				prev_pkt = pkt;
				return enqueue_connection(connection);
			}
			return NOMEM;
		}
		case NGTCP2_ERR_VERSION_NEGOTIATION:
		{
			return send_version_negotiation_packet(vcid->dcid, vcid->dcidlen, vcid->scid, vcid->scidlen);
		}
		default:
		{
			return res;
		}
	}
}

static inline int resolve_error(int result_code)
{
	switch (result_code)
	{
		case -ENOBUFS:
		{
			return 0;
		}
		case EAGAIN: // The socket is marked non-blocking and no data is available to be read.
		case EWOULDBLOCK:
		{
			return prepare_read();
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

static inline int enqueue_connection(struct connection *connection)
{
	pthread_mutex_lock(&mutex);

	if (IsEmpty(available_queue))
	{
		res = send_signal(&cond);
		if (res != 0)
		{
			pthread_mutex_unlock(&mutex);
			return res;
		}
	}

	if (IsEmpty(connection->tasks))
	{
		Enqueue(available_queue, connection);
	}

	Enqueue(connection->tasks, task);

	pthread_mutex_unlock(&mutex);
	return 0;
}

int dequeue_connection(struct connection *connection)
{
	pthread_mutex_lock(&mutex);

	while (IsEmpty(available_queue))
	{
		res = pthread_cond_wait(&cond, &mutex);
		if (res != 0)
		{
			pthread_mutex_unlock(&mutex);
			return res;
		}
	}

	Dequeue(new_conn_queue, task);

	pthread_mutex_unlock(&mutex);
	return 0;
}