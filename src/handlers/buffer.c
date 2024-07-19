// Copyright (C) 2024, Moritz Scheer

#include <linux/io_uring.h>
#include <liburing.h>
#include "buffer.h"
#include "../includes/ring.h"

int setup_buffer_pool()
{
	buf_ring = mmap(NULL, BUFFER_RING_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	if (buf_ring == MAP_FAILED)
	{
		return -1;
	}

	io_uring_buf_ring_init(buf_ring);

	reg = (struct io_uring_buf_reg) {
		.ring_addr = (unsigned long) buf_ring,
		.ring_entries = BUFFERS,
		.bgid = 0
	};

	buffer_base = (unsigned char *) buf_ring + sizeof(struct io_uring_buf) * BUFFERS;

	res = io_uring_register_buf_ring(&ring, &reg, 0);
	if (res)
	{
		return res;
	}

	for (int i = 0; i < BUFFERS; i++)
	{
		io_uring_buf_ring_add(buf_ring,
		                      get_buffer(ctx, i),
		                      buffer_size(ctx),
		                      i,
		                      io_uring_buf_ring_mask(BUFFERS),
		                      i);
	}
	io_uring_buf_ring_advance(buf_ring, BUFFERS);

	return 0;
}

unsigned char *get_buffer(int idx)
{
	return buffer_base + (idx << buf_shift);
}