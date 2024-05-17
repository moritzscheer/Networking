// Copyright (C) 2024, Moritz Scheer

#include "write.h"
#include "../includes/server.h"
#include "../includes/io_buffer.h"

int handle_write_result(server *server, io_buffer *io_buffer, int result)
{
	if (bytes_written == -1)
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

int *prepare_write(io_uring *ring, int socket, io_buffer *io_buffer)
{
	io_buffer->event_type = WRITE;

	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
	if(sqe == NULL)
		return QUEUE_FULL;

	io_uring_prep_writev(sqe, socket, io_buffer->iov, io_buffer->iovec_count, 0);
	io_uring_sqe_set_data(sqe, io_buffer);
	return 0;
}