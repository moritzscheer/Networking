// Copyright (C) 2024, Moritz Scheer

#include <linux/io_uring.h>
#include <liburing.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <ngtcp2/ngtcp2.h>

#include "read.h"
#include "read_queue.h"
#include "../connection.h"
#include "../../core/threads.h"
#include "../../includes/ring.h"
#include "../../includes/server.h"
#include "../../includes/status.h"
#include "../../middleware/ngtcp2/packet.h"
#include "../../utils/queue.h"
#include "../../utils/utils.h"

int prepare_read(void)
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

int validate_read(struct io_uring_cqe *cqe)
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
			return 0;
		}

		if (msg_out->namelen > msg.msg_namelen)
		{
			recycle_buffer(ctx, idx);
			return -1;
		}

		uint8_t *pkt = io_uring_recvmsg_payload(msg_out);
		size_t pktlen = io_uring_recvmsg_payload_length(&msg);
		struct sockaddr_storage *addr = io_uring_recvmsg_name(msg_out);
		socklen_t addrlen = get_addrlen(addr);

		return resolve_success(pkt, pktlen, addr, addrlen);
	}
	return resolve_error(cqe->res);
}

static int resolve_success(void *pkt, size_t pktlen, struct sockaddr_storage *addr, socklen_t addrlen)
{
	ngtcp2_version_cid vcid;

	res = ngtcp2_pkt_decode_version_cid(&vcid, pkt, pktlen, NGTCP2_MAX_CIDLEN);
	if (res == NGTCP2_ERR_VERSION_NEGOTIATION)
	{
		return send_version_negotiation_packet(vcid->dcid, vcid->dcidlen, vcid->scid, vcid->scidlen);
	}
	else if (res != 0)
	{
		return DROP;
	}

	struct read_event *event = create_read_event(&vcid, pkt, pktlen, addr);
	if (!event)
	{
		return ENOMEM;
	}

	struct connection *connection = NULL;
	pthread_mutex_lock(&conn_mutex);

	res = get_connection(connection, event);
	if (!connection)
	{
		free(event);
		return unlock_and_return(&mutex, res);
	}

	enqueue_read_event(connection, event);
	return unlock_and_return(&conn_mutex, res);
}

static int resolve_success(void *pkt, size_t pktlen, struct sockaddr_storage *addr, socklen_t addrlen)
{
	struct read_event_storage event;
	res = decode_header(&event, pkt, pktlen, addr, addrlen);
	if (res != 0)
	{
		return res;
	}

	struct connection *connection = NULL;
	pthread_mutex_lock(&conn_mutex);

	res = get_connection(connection, event);
	if (!connection)
	{
		free(event);
		return unlock_and_return(&mutex, res);
	}

	enqueue_read_event(connection, event);
	return unlock_and_return(&conn_mutex, res);
}

static int resolve_error(int result_code)
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

static struct read_event *create_read_event(ngtcp2_version_cid *vcid, void *pkt, size_t pktlen,
                                            struct sockaddr_storage *addr)
{
	struct read_event *event = calloc(1, sizeof(struct read_event));
	if (!event)
	{
		return NULL;
	}

	ngtcp2_cid_init(&event->dcid, vcid->dcid, vcid->dcidlen);
	if (vcid->scid)
	{
		ngtcp2_cid_init(&event->scid, vcid->scid, vcid->scidlen);
	}

	event->version = vcid->version;
	event->scid = scid;
	event->receive_time = get_timestamp_ns();
	event->iov_base = iov_base;
	event->iov_len = iov_len;
	event->prev_recv_pkt = prev_read_event;

	prev_read_event = event;
	return event;
}

static void enqueue_read_event(struct connection *connection, struct read_event *event)
{
	pthread_mutex_lock(&read_mutex);

	if (!connection->read_events->head)
	{
		enqueue(available_conn, connection);
	}
	enqueue(connection->read_events, event);

	if (available_conn->head && available_threads > 0)
	{
		pthread_cond_signal(cond_var);
	}

	pthread_mutex_unlock(&read_mutex);
}

int dequeue_event(struct connection *connection, struct read_event *event)
{
	pthread_mutex_lock(&read_mutex);

	while (!available_queue->head)
	{
		res = pthread_cond_wait(&cond, mutex);
		if (res != 0)
		{
			return unlock_and_return(&read_mutex, res);
		}
	}
	connection = dequeue(available_queue);
	enqueue(blocked_queue, connection);
	connection->blocked = true;

	event = dequeue(connection->packets);

	return unlock_and_return(&read_mutex, 0);
}

void finish_event(struct connection *connection, struct read_event *event)
{
	pthread_mutex_lock(&mutex);

	if (!is_empty(connection->packets))
	{

	}
	connection->blocked = false;

	pthread_mutex_unlock(&mutex);
	free(event);
}