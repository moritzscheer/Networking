// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "buffer.h"
#include "../core/threads.h"
#include "../../lib/uthash/utstack.h"
#include "../includes/server.h"
#include "../core/network.h"

static pthread_cond_t cond_buf_empty;
static bool buf_empty = false;

static int buffer_index = 0;
static struct msghdr *buffers[INITIAL_SIZE];

int create_buffer_pool(void)
{
	res = pthread_cond_init(&cond_buf_empty);
	if (res != 0)
	{
		return res;
	}

	while (buffer_index < INITIAL_SIZE)
	{
		struct msghdr *buffer = allocate_buffer();
		if (buffer == NULL)
		{
			destroy_buffer_pool();
			return ENOMEM;
		}
		buffers[buffer_index] = buffer;
		buffer_index++;
	}
	return 0;
}

static struct msghdr *allocate_buffer(void)
{
	void *buffer = calloc(1, BUFFER_SIZE_TOTAL);
	if (buffer == NULL)
	{
		perror("Memory allocation failed");
		return NULL;
	}

	struct msghdr *msghdr = MSGHDR_START(buffer);

	msghdr->msg_control = CMSGHDR_START(buffer);
	msghdr->msg_controllen = CMSGHDR_LEN;

	msghdr->msg_iov = IOVEC_START(buffer);
	msghdr->msg_iovlen = IOV_NUM;

	void *base = IOV_BASE_START(buffer);
	for (int i = 0; i < IOV_NUM; i++)
	{
		msghdr->msg_iov[i].iov_base = (char *) base + (i * IOV_LEN);
		msghdr->msg_iov[i].iov_len = IOV_LEN;
	}

	return msghdr;
}

void destroy_buffer_pool(void)
{
	for (int i = 0; i < buffer_index; i++)
		free(buffers[i]);
	buffer_index = 0;
}

int get_buffer(struct msghdr *message)
{
	pthread_mutex_lock(&mutex);
	while (1)
	{
		if (buffer_index > 0)
		{
			buffer_index--;
			message = buffers[buffer_index];
			pthread_mutex_unlock(&mutex);
			return 0;
		}

		res = pthread_cond_wait(&cond_buffer_available, &mutex);
		if (res != 0)
		{
			pthread_mutex_unlock(&mutex);
			return res;
		}
	}
}

void return_buffer(struct msghdr *msghdr)
{
	pthread_mutex_lock(&mutex);
	if (msghdr != NULL && buffer_index < INITIAL_SIZE)
	{
		reset_buffer(msghdr);
		buffers[buffer_index] = msghdr;
		buffer_index++;
		if (buffer_index == 1)
		{
			pthread_cond_signal(&cond_buffer_available);
		}
	}
	pthread_mutex_unlock(&mutex);
}

static void reset_buffer(struct msghdr *msghdr)
{
	if (msghdr != NULL)
	{
		msghdr->msg_name = NULL;
		msghdr->msg_namelen = 0;
		msghdr->msg_flags = 0;
		memset(CMSGHDR_START(msghdr), 0, CMSGHDR_LEN);
		memset(IOV_BASE_START(msghdr), 0, IOV_BASE_LEN);
	}
}

static int setup_buffer_pool(struct ctx *ctx)
{
	int res, I;
	void *mapped;
	struct io_uring_buf_reg reg =
		{
			.ring_addr = 0,
			.ring_entries = BUFFERS,
			.bgid = 0
		};

	ctx->buf_ring_size = (sizeof(struct io_uring_buf) + buffer_size(ctx)) * BUFFERS;
	mapped = mmap(NULL, ctx->buf_ring_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	if (mapped == MAP_FAILED)
	{
		return -1;
	}
	ctx->buf_ring = (struct io_uring_buf_ring *) mapped;

	io_uring_buf_ring_init(ctx->buf_ring);

	reg = (struct io_uring_buf_reg)
		{
			.ring_addr = (unsigned long) ctx->buf_ring,
			.ring_entries = BUFFERS,
			.bgid = 0
		};
	ctx->buffer_base = (unsigned char *) ctx->buf_ring +
	                   sizeof(struct io_uring_buf) * BUFFERS;

	res = io_uring_register_buf_ring(ring, &reg, 0);
	if (res)
	{
		return res;
	}

	for (I = 0; I < BUFFERS; I++)
	{
		io_uring_buf_ring_add(ctx->buf_ring, get_buffer(ctx, I), buffer_size(ctx), I,
		                      io_uring_buf_ring_mask(BUFFERS), I);
	}
	io_uring_buf_ring_advance(ctx->buf_ring, BUFFERS);

	return 0;
}