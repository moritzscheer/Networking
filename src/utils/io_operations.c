// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <liburing.h>

void prep_read_io(io_uring *ring, int server_socket)
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
	if(!sqe)
	{
		perror("Failed to get SQE");
		return errno;
	}

	IORequest *request = malloc(REQUEST_SIZE);
	if (!request) p_return("Failed to allocate buffer for request struct")

	uint8_t *buffer = malloc(BUFSIZE)
	if (!buffer) p_return("Failed to allocate buffer for iov_base")
	memset(buffer, 0, BUFSIZE);

	request->iov[0].iov_base = buffer;
	request->iov[0].iov_len = BUFSIZE;
	request->event_type = READ;

	io_uring_prep_read(sqe, server_socket, request->iov, request->iov[0].iov_len, 0);
	io_uring_sqe_set_data(sqe, request);
}

void prep_write_io(io_uring *ring, IORequest *request)
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
	if(!sqe)
	{
		perror("Failed to get SQE");
		return errno;
	}

	request->event_type = WRITE;

	io_uring_prep_writev(sqe, request->client_socket, response->iov, response->iovec_count, 0);
	io_uring_sqe_set_data(sqe, response);
}

void prep_close_io(io_uring *ring, IORequest *request)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	if(!sqe)
	{
		perror("Failed to get SQE");
		return errno;
	}

    request->event_type = CLOSE;

	io_uring_prep_close(sqe, request->client_socket);
    io_uring_sqe_set_data(sqe, request);
}