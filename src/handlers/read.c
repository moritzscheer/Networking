// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "read.h"
#include "../handlers/parser.h"
#include "../handlers/buffer.h"
#include "../includes/message.h"
#include "../includes/status.h"
#include "../includes/server.h"


int handle_read_result(server *server, message *message, int result)
{
	if (result > 0)
		return parse(server, message);
	else if (result < 0)
		switch (result)
		{
			case EINTR: // Read operation interrupted by signal
				return RETRY;
			case EAGAIN: //
			case EWOULDBLOCK:
				fprintf(stderr, "No data available to read (non-blocking mode)\n");
				break;
			case EBADF:
				fprintf(stderr, "Invalid file descriptor\n");
				break;
			default:
				return DONT_RESPOND;
		}
	else
	{
		if (end_of_stream())
			close_stream(message);
		if (empty_packet(message))
			return DONT_RESPOND;
		else
	}
}

int prepare_read(io_uring *ring, int socket, int *submissions, struct msghdr *message)
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

	set_event_type(message, READ);
	(*submissions)++;
	io_uring_prep_recvmsg(sqe, socket, message, 0);
	io_uring_sqe_set_data(sqe, message);
	return 0;
}