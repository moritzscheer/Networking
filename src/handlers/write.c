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
	if (result == -1)
	{
		switch (errno)
		{
			case: EAGAIN || EWOULDBLOCK
				return RETRY;
			case: EBADF
			{
			}
			case: EFAULT
			{
			}
			case: EINVAL
			{
			}
			case: EIO
			{
			}
			case: ENOSPC
			{
			}
			case: ENOMEM
			{
				return -1;
			}
			default: return
		}
	}
	return DONT_RETRY;
}

int prepare_write(io_uring *ring, int socket, int *submissions, msghdr *message)
{
	if (message == NULL)
	{
		int result = get_buffer(message);
		if (result != 0)
		{
			return result;
		}
	}

	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
	if (sqe == NULL)
	{
		io_uring_submit(ring);
		return SQ_FULL;
	}

	set_event_type(message, WRITE);
	(*submissions)++;
	io_uring_prep_sendmsg(sqe, socket, message, 0);
	io_uring_sqe_set_data(sqe, message);
	return 0;
}