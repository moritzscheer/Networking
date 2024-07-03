// Copyright (C) 2024, Moritz Scheer

#include "io_uring.h"

int setup_ring(struct io_uring *ring, int socket)
{
	struct io_uring_params params;
	int res;

	memset(&params, 0, sizeof(params));
	params.cq_entries = QD * 8;
	params.flags = IORING_SETUP_SUBMIT_ALL | IORING_SETUP_COOP_TASKRUN |
	               IORING_SETUP_CQSIZE | IORING_SETUP_SINGLE_ISSUER;

	res = io_uring_queue_init_params(QD, ring, &params);
	if (res != 0)
	{
		return res;
	}

	res = setup_buffer_pool();
	if (res != 0)
	{
		io_uring_queue_exit(ring);
	}

	memset(msg, 0, sizeof(msg));
	msg.msg_namelen = sizeof(struct sockaddr_storage);
	msg.msg_controllen = CONTROLLEN;

	res = io_uring_register_files(&ring, &socket, 1);
	if (res != 0)
	{
		io_uring_queue_exit(ring);
		return res;
	}

	res = prepare_read(ring, socket);
	if (res == -1)
	{
		io_uring_queue_exit(ring);
		return res;
	}

	return res;
}

static int setup_buffer_pool(struct ctx *ctx)
{
	int ret, i;
	void *mapped;
	struct io_uring_buf_reg reg = {.ring_addr = 0,
		.ring_entries = BUFFERS,
		.bgid = 0};

	ctx->buf_ring_size = (sizeof(struct io_uring_buf) + buffer_size(ctx)) * BUFFERS;
	mapped = mmap(NULL, ctx->buf_ring_size, PROT_READ | PROT_WRITE,
	              MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	if (mapped == MAP_FAILED)
	{
		fprintf(stderr, "buf_ring mmap: %s\n", strerror(errno));
		return -1;
	}
	ctx->buf_ring = (struct io_uring_buf_ring *) mapped;

	io_uring_buf_ring_init(ctx->buf_ring);

	reg = (struct io_uring_buf_reg) {
		.ring_addr = (unsigned long) ctx->buf_ring,
		.ring_entries = BUFFERS,
		.bgid = 0
	};
	ctx->buffer_base = (unsigned char *) ctx->buf_ring +
	                   sizeof(struct io_uring_buf) * BUFFERS;

	ret = io_uring_register_buf_ring(&ctx->ring, &reg, 0);
	if (ret)
	{
		fprintf(stderr, "buf_ring init failed: %s\n"
		                "NB This requires a kernel version >= 6.0\n",
		        strerror(-ret));
		return ret;
	}

	for (i = 0; i < BUFFERS; i++)
	{
		io_uring_buf_ring_add(ctx->buf_ring, get_buffer(ctx, i), buffer_size(ctx), i,
		                      io_uring_buf_ring_mask(BUFFERS), i);
	}
	io_uring_buf_ring_advance(ctx->buf_ring, BUFFERS);

	return 0;
}

int cleanup_ring(int result)
{
	io_uring_queue_exit(ring);
	return result;
}