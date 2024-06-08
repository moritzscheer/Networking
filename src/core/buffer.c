// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "buffer.h"
#include "threads.h"
#include "../../lib/uthash/utstack.h"
#include "../includes/server.h"
#include "network.h"

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