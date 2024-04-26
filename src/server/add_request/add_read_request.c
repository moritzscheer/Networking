// Copyright (C) 2024 Moritz Scheer

#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <liburing.h>
#include "../request.h"

int add_read_request(int client_socket)
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	Request *client_request = malloc(sizeof(struct request) + sizeof(struct iovec));

	req->event_type = EVENT_TYPE_READ;
	req->client_socket = client_socket;
	req->io[0].iov_base = malloc(READ_SZ);
	req->io[0].iov_len = READ_SZ;

	memset(req->iov[0].iov_base, 0, READ_SZ);
	io_uring_prep_readv(sqe, client_socket, &req->iov[0], 1, 0);
	io_uring_sqe_set_data(sqe, req);
	io_uring_submit(&ring);
	return 0;
}
