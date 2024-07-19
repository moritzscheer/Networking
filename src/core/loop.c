// Copyright (C) 2024, Moritz Scheer

#include <linux/io_uring.h>
#include <liburing.h>
#include <sys/socket.h>
#include <string.h>
#include "loop.h"
#include "../handlers/read.h"
#include "../handlers/write.h"
#include "../handlers/buffer.h"
#include "../includes/server.h"
#include "../includes/status.h"
#include "../includes/ring.h"

int listen_loop()
{
	int i, count;

	res = setup_ring();
	if (res != 0)
	{
		return res;
	}

	while (1)
	{
		res = io_uring_submit_and_wait(&ring, 1);
		if (res == -EINTR)
		{
			continue;
		}
		else if (res < 0)
		{
			return cleanup_ring(res);
		}

		count = io_uring_peek_batch_cqe(&ring, &cqes[0], CQES);
		for (i = 0; i < count; i++)
		{
			res = check_type(cqes[i], io_uring_cqe_get_data(cqes[i]));
			if (res != 0)
			{
				return cleanup_ring(res);
			}
		}
		io_uring_cq_advance(&ring, count);
	}
}

static inline int validate_cqe(struct io_uring_cqe *cqe, uint64_t *event_type)
{
	if (!(cqe->flags & IORING_CQE_F_MORE))
	{
		res = prepare_read();
		if (res != 0)
		{
			return res;
		}
	}

	switch (event_type)
	{
		case READ:
		{
			return validate_read(cqe);
		}
		case WRITE:
		{
			return validate_write(cqe);
		}
		default:
		{
			return -EINVAL;
		}
	}
}

static int setup_ring()
{
	struct io_uring_params params;
	memset(&params, 0, sizeof(params));
	params.cq_entries = SQES * 8;
	params.flags = IORING_SETUP_SUBMIT_ALL | IORING_SETUP_COOP_TASKRUN |
	               IORING_SETUP_CQSIZE | IORING_SETUP_SINGLE_ISSUER;

	res = io_uring_queue_init_params(SQES, &ring, &params);
	if (res != 0)
	{
		return res;
	}

	res = setup_buffer_pool();
	if (res != 0)
	{
		io_uring_queue_exit(&ring);
		return res;
	}

	memset(msg, 0, sizeof(msg));
	msg.msg_namelen = sizeof(struct sockaddr_storage);
	msg.msg_controllen = CONTROLLEN;

	res = io_uring_register_files(&ring, &socket, 1);
	if (res != 0)
	{
		io_uring_queue_exit(&ring);
		return res;
	}

	res = prepare_read();
	if (res == -1)
	{
		io_uring_queue_exit(&ring);
		return res;
	}

	return res;
}

static int cleanup_ring(int result)
{
	io_uring_queue_exit(&ring);
	return result;
}