// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <errno.h>

#include "read.h"
#include "../handlers/parser.h"
#include "../includes/server.h"
#include "../includes/io_buffer.h"
#include "../includes/errno2.h"


int handle_read_result(server *server, io_buffer *io_buffer, int result)
{
	if (result > 0)
		return parse(server, io_buffer);
	else if (result < 0)
		return handle_read_error(server, io_buffer, result);
	else
	{
		if (end_of_stream())
			close_stream(io_buffer);
		if (empty_packet(io_buffer))
			return DONT_RESPOND;
		else
	}
}

int *prepare_read(io_uring *ring, int socket, io_buffer *io_buffer)
{
	if (io_buffer == NULL)
	{
		io_buffer = malloc(sizeof(uint8_t) * REQUEST_SIZE);
		if (io_buffer == NULL)
		{
			perror("Failed to allocate memory for IO Buffer");
			return errno;
		}

		memset(io_buffer, 0, REQUEST_SIZE);
		for (int i = 0; i < MAX_IOV; i++)
		{
			io_buffer->iov[i].iov_base = io_buffer->buffer[i];
			io_buffer->iov[i].iov_len = BUFFER_SIZE;
		}
	}

	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
	if (sqe == NULL)
		return QUEUE_FULL;

	io_uring_prep_readv(sqe, socket, io_buffer->iov, MAX_IOV, 0);
	io_uring_sqe_set_data(sqe, io_buffer);
	return 0;
}

int handle_read_error(server *server, io_buffer *io_buffer, int result)
{
	io_buffer->tries = (!io_buffer->tries) ? 0 : io_buffer->tries++

	switch (result) {
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
}