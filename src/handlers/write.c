// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "write.h"
#include "../handlers/parser.h"
#include "../handlers/buffer.h"
#include "../includes/message.h"
#include "../includes/status.h"
#include "../includes/server.h"

int handle_write_result(server *server, message *message, int result)
{
	if (result < 0)
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
	return DONT_RETRY;
}

int prepare_write(io_uring *ring, int socket, msghdr *message)
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
	if (sqe == NULL)
	{
		return SQ_FULL;
	}

	set_event_type(message, WRITE);

	io_uring_prep_sendmsg(sqe, socket, message, 0);
	io_uring_sqe_set_data(sqe, message);
	return 0;
}