// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "read.h"
#include "../core/buffer.h"
#include "../core/threads.h"
#include "../handlers/parser.h"
#include "../includes/message.h"
#include "../includes/status.h"
#include "../includes/server.h"

int prepare_read(io_uring *ring, int socket, struct msghdr *message)
{
	/* Gets the free buffer from the buffer pool and waits if none is free */
	if (message == NULL)
	{
		int res = get_buffer(message);
		if (res != 0)
		{
			return res;
		}
	}

	/* Gets the Submission Queue from the ring and submits it, if the queue is full */
	struct io_uring_sqe *sqe;
	while (1)
	{
		sqe = io_uring_get_sqe(ring);
		if (sqe != NULL)
		{
			break;
		}

		io_uring_submit(ring);
		usleep(1000);
	}

	/* Get ancillary data from message struct and handles them */
	uint8_t tries = GET_TRIES(message);
	if (tries == 0)
	{
		uint8_t event_type = GET_EVENT_TYPE(message);
		event_type = READ;
	}
	tries++;

	io_uring_prep_recvmsg(sqe, socket, message, 0);
	io_uring_sqe_set_data(sqe, message);
	return 0;
}

int resolve_read(server *server, message *message, int result)
{
	if (result >= 0)
	{
		result = parse_packet(server, message);
	}

	switch (result)
	{
		case SUCCESSFUL:
		{
			return RESPOND;
		}
		case END_OF_STREAM:
		{
			close_stream(message);
			return RESPOND;
		}
		case END_OF_PACKET:
		{
			return RESPOND;
		}
		case -EAGAIN || -EWOULDBLOCK:
		{
			return RETRY;
		}
		case -EBADF:
		{
			return RETRY;
		}
		case -EFAULT:
		{
			return RETRY;
		}
		case -EINVAL:
		{
			return RETRY;
		}
		case -EIO:
		{
			return RETRY;
		}
		case -ENOSPC:
		{
			return RETRY;
		}
		case -ENOMEM:
		{
			return DROP;
		}
		default:
		{
			return DROP;
		}
	}
}