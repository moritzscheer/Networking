// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "buffer.h"
#include "../../lib/uthash/utstack.h"
#include "../includes/server.h"
#include "../core/socket.h"

static size_t buffer_index = 0;
static struct msghdr *buffers[INITIAL_SIZE];

extern int create_buffer_pool(void)
{
	while (buffer_index < INITIAL_SIZE)
	{
		struct msghdr *buffer = allocate_buffer();
		if (buffer == NULL)
		{
			destroy_buffer_pool();
			return errno;
		}
		buffers[buffer_index] = buffer;
		buffer_index++;
	}
	return 0;
}

extern struct msghdr *get_buffer(void)
{
	if (buffer_index > 0)
	{
		buffer_index--;
		return buffers[buffer_index];
	}
	return NULL;
}

void set_event_type(struct msghdr *msghdr, type)
{
	uint8_t event_type = GET_EVENT_TYPE(msghdr);
	*type = event_type;
}

void increment_tries(struct msghdr *msghdr)
{
	uint8_t tries = GET_TRIES(msghdr);
	*type = tries++;
}

extern void return_buffer(struct msghdr *msghdr)
{
	if (msghdr != NULL && buffer_index < INITIAL_SIZE)
	{
		reset_buffer(msghdr);
		buffers[buffer_index] = msghdr;
		buffer_index++;
	}
}

extern void destroy_buffer_pool(void)
{
	for (int i = 0; i < buffer_index; i++)
		free(buffers[i]);
	buffer_index = 0;
}

static struct msghdr *allocate_buffer(void)
{
	size_t buffer_size = MSGHDR_LEN + CMSGHDR_LEN + IOVEC_LEN + IOV_BASE_LEN;

	void *buffer = calloc(1, buffer_size);
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