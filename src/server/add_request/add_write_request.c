// Copyright (C) 2024 Moritz Scheer

#include <liburing.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "../request.h"

int add_write_request(struct request *req)
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);

	req->event_type = EVENT_TYPE_WRITE;

	io_uring_prep_writev(sqe, req->client_socket, req->iov, req->iovec_count, 0);
	io_uring_sqe_set_data(sqe, req);
	io_uring_submit(&ring);
	return 0;
}
