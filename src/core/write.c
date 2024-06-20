// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "write.h"
#include "../handlers/parser.h"
#include "../handlers/buffer.h"
#include "threads.h"
#include "../includes/message.h"
#include "../includes/status.h"
#include "../includes/server.h"

pthread_mutex_t write_mutex;

void write_loop(struct server *server)
{
	int res, status_code;

	/*
	 * Main server loop
	 */
	while (1)
	{
		res = io_uring_wait_cqe(server->ring, &cqe);
		if (res != 0)
		{
			break;
		}

		/*
		 * Loop over completed events
		 */
		while (1)
		{
			struct msghdr *message = (struct msghdr *) cqe->user_data;

			resolve_read(server->socket, message, status_code);

			io_uring_cqe_seen(ring, cqe);

			if (io_uring_peek_cqe(ring, &cqe) == -EAGAIN)
			{
				break;
			}
		}

	}
	return res;
}

int prepare_write(io_uring *ring, int socket, struct msghdr *message)
{
	/* Gets the Submission Queue and submits it, if the queue is full */
	struct io_uring_sqe *sqe;
	while (1)
	{
		sqe = io_uring_get_sqe(ring);
		if (sqe != NULL)
		{
			break;
		}

		io_uring_submit_and_wait(ring, 2);
	}

	/* Get ancillary data from message struct and handles them */
	uint8_t tries = GET_TRIES(message);
	if (tries == 0)
	{
		uint8_t event_type = GET_EVENT_TYPE(message);
		event_type = WRITE;
	}
	tries++;

	io_uring_prep_sendmsg_zc(sqe, socket, message, 0);
	io_uring_sqe_set_data(sqe, message);
	return 0;
}

int resolve_write(server *server, message *message, int result)
{
	switch (result)
	{
		case EAGAIN: // The socket is marked non-blocking and the operation would block.
		case EWOULDBLOCK:
		{
			return handle_retry_logic();
		}
		case EINTR: // The call was interrupted by a signal before any data was transmitted.
		{
			return handle_interrupted_call();
		}
		case EPIPE: // The local end has been shut down on a connection oriented socket.
		{
			return handle_connection_closed();
		}
		case ENOTCONN: // The socket is not connected.
		case EHOSTUNREACH: // The destination host is unreachable.
		case ENETDOWN: // The network is down.
		case ENETUNREACH: // The network is unreachable.
		{
			return handle_wait_and_retry();
		}
		case ECONNRESET: // Connection reset by peer.
		{
			return handle_connection_reset();
		}
		case EDESTADDRREQ: // The socket is not connection-mode, and no peer address is set.
		{
			return handle_missing_address();
		}
		case EFAULT: // The msg argument points outside the process's address space.
		{
			return handle_fault();
		}
		case EINVAL: // Invalid argument passed.
		case EMSGSIZE: // The socket requires that message be sent atomically, and the size of the message to be sent made this impossible.
		case ENOBUFS: // Insufficient resources are available in the system to complete the call.
		case ENOMEM: // No memory available to complete the operation.
		{
			return handle_retry_or_abort();
		}
		case ENOTSOCK: // The file descriptor is not associated with a socket.
		{
			return handle_not_socket();
		}
		case EOPNOTSUPP: // The socket does not support the specified operation.
		{
			return handle_operation_not_supported();
		}
		case EACCES: // The caller does not have the required permissions.
		{
			return handle_permission_denied();
		}
		case EIO: // An I/O error occurred.
		default:
		{
			return handle_unknown_error();
		}
	}
}

void write_version_negotiation_packet()
{

}