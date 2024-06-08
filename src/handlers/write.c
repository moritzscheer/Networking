// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "write.h"
#include "../handlers/parser.h"
#include "../core/buffer.h"
#include "../core/threads.h"
#include "../includes/message.h"
#include "../includes/status.h"
#include "../includes/server.h"

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

		io_uring_submit(ring);
		usleep(1000);
	}

	/* Get ancillary data from message struct and handles them */
	uint8_t tries = GET_TRIES(message);
	if (tries == 0)
	{
		uint8_t event_type = GET_EVENT_TYPE(message);
		event_type = WRITE;
	}
	tries++;

	io_uring_prep_sendmsg(sqe, socket, message, 0);
	io_uring_sqe_set_data(sqe, message);
	return 0;
}

int resolve_write(server *server, message *message, int result)
{
	if (result >= 0)
	{
		result = parse_packet(server, message);
	}

	// resolve read error code
	switch (result)
	{
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