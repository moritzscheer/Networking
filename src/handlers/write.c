// Copyright (C) 2024, Moritz Scheer

#include <linux/io_uring.h>
#include <liburing.h>
#include <errno.h>

#include "write.h"
#include "../core/loop.h"
#include "../includes/status.h"
#include "../includes/ring.h"
#include "../includes/server.h"

int prepare_write(const uint8_t *buf, size_t len)
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	if (!sqe)
	{
		io_uring_submit(&ring);
		sqe = io_uring_get_sqe(&ring);
		if (!sqe)
		{
			return -1;
		}
	}

	io_uring_prep_send_zc(sqe, socket, buf, len, MSG_TRUNC);
	sqe->flags |= IOSQE_FIXED_FILE;

	sqe->flags |= IOSQE_BUFFER_SELECT;
	sqe->buf_group = 0;

	io_uring_sqe_set_data(sqe, WRITE);
	return 0;
}

inline int validate_write(struct io_uring_cqe *cqe)
{
	if (!(cqe->flags & IORING_CQE_F_MORE))
	{
		res = prepare_read();
		if (res != 0)
		{
			return res;
		}
	}

	/* If operation was successful */
	if (cqe->res >= 0 && (cqe->flags & IORING_CQE_F_BUFFER))
	{
		idx = cqe->flags >> 16;

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
	return 0;
}

static inline int resolve_write(int socket, int result)
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