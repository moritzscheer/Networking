// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "buffer2.h"
#include "../../lib/uthash/utstack.h"
#include "../includes/server.h"
#include "../core/socket.h"

Buffer *buffer_pool = NULL;

int create_buffer_pool()
{
	for (int I = 0; I < INITIAL_SIZE; I++)
	{
		Buffer *buffer = allocate_buffer();
		if (buffer == NULL)
			return errno;
		STACK_PUSH(buffer_pool, buffer);
	}
	return 0;
}

Buffer *get_buffer()
{
	if (STACK_EMPTY(buffer_pool))
		return NULL;
	Buffer *msg;
	STACK_POP(buffer_pool, msg);
	return msg;
}

void return_buffer(Buffer *msg)
{
	reset_buffer(msg);
	STACK_PUSH(buffer_pool, msg);
}

void destroy_buffer_pool()
{
	for (int I = 0; I < INITIAL_SIZE; I++)
	{
		Buffer *msg;
		STACK_POP(buffer_pool, msg);
		free_buffer(msg);
	}
}

struct msghdr *allocate_buffer()
{
	struct msghdr *msghdr = calloc(1, sizeof(struct msghdr));
	if (msghdr == NULL)
	{
		perror("Memory allocation failed");
		free(buffer);
		return NULL;
	}

	msghdr->msg_control = init_cmsghdr(sizeof(Extra_Data));
	if (msghdr->msg_control == NULL)
	{
		free(msghdr);
		free(buffer);
		return NULL;
	}
	msghdr->msg_controllen = sizeof(Extra_Data);

	msghdr->msg_iov = init_iovec(msghdr, NGTCP2_MAX_UDP_PAYLOAD_SIZE, NUM_IOVEC);
	if (msghdr->msg_iov == NULL)
	{
		free(msghdr->msg_control);
		free(msghdr);
		free(buffer);
		return NULL;
	}
	msghdr->msg_iovlen = NUM_IOVEC;

	return buffer;
}

static void free_buffer(Buffer *msg)
{
	if (msg == NULL) return;
	if (msg->msg_iov != NULL)
	{
		for (int I = 0; I < msg->msg_iovlen; I++)
		{
			if (msg->msg_iov[I].iov_base != NULL)
				free(msg->msg_iov[I].iov_base);
		}
		free(msg->msg_iov);
	}
	free(msg);
}

static void reset_buffer(Buffer *msg)
{
	msg->event_type = 0;
	msg->tries = 0;

	struct iovec *tmp = msg->msg_iov;
	memset(msg->msghdr, 0, sizeof(struct msghdr));
	msg->msg_iov = tmp;
	msg->msg_iovlen = NUM_IOVEC;

	for (int I = 0; I < NUM_IOVEC; I++)
		if (msg->msg_iov[I].iov_base != NULL)
			memset(msg->msg_iov[I].iov_base, 0, NGTCP2_MAX_UDP_PAYLOAD_SIZE);
}