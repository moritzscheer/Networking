
#include <linux/types.h>
#include <linux/io_uring.h>
#include <sys/socket.h>
#include <liburing.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "loop.h"
#include "../core/threads.h"
#include "../handlers/read.h"
#include "../handlers/write.h"
#include "../handlers/buffer.h"
#include "../includes/server.h" -
#include "../includes/message.h"
#include "../includes/status.h"

static struct io_uring *ring;
static struct io_uring_cqe *cqe;

void server_loop(Server *server)
{
	int result;

	result = io_uring_queue_init(QUEUE_DEPTH, ring, 0);
	if (result != 0)
	{
		perror("Could not initialize io_uring queue.");
		return result;
	}

	result = prepare_read(ring, server->socket);
	if (result != 0)
	{
		io_uring_queue_exit(ring);
		return result;
	}

	while (1)
	{
		result = io_uring_wait_cqe(ring, &cqe);
		if (result != 0)
		{
			perror("waiting for completion");
			break;
		}

		result = start_handling_tasks();
		if (result != 0)
		{
			break;
		}
	}

	io_uring_queue_exit(ring);
	return result;
}

void *thread_function()
{
	bool wait_is_necessary = true;
	int result = 0;

	/*
	 * this infinite loop is necessary for the thread to continuously waiting for new tasks.
	 */
	while (1)
	{
		struct msghdr *message;
		result = get_new_task(ring, cqe, &wait_is_necessary, message);
		if (result != 0)
		{
			return result;
		}

		switch (GET_EVENT_TYPE(message))
		{
			case READ:
			{
				int result = handle_read(server, message, cqe->result);
				switch (result)
				{
					case RESPOND: prepare_write(ring, server->socket, &submissions, message);
						break;
					case RETRY: prepare_read(ring, server->socket, &submissions, message);
						break;
					default: return_buffer(message);
						return result;
				}
				result = prepare_read(ring, server->socket, &submissions);
				if (result != 0) return result;
				break;
			}
			case WRITE:
			{
				int result = handle_write_result(server, message, cqe->result);
				switch (result)
				{
					case RETRY: result = prepare_write(ring, server->socket, &submissions, message);
						if (result != 0) return result;
						break;
					default: return_buffer(message);
						break;
				}
				break;
			}
			default: perror("Unexpected request type");
				break;
		}

		result = finish_task(&wait_is_necessary);
		if (result != 0)
		{
			return result;
		}
	}
}