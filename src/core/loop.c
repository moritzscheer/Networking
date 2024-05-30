
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
#include "../includes/server.h"
#include "../includes/status.h"

static struct io_uring *ring;
static struct io_uring_cqe *cqe;

void server_loop(Server *server)
{
	int res = io_uring_queue_init(QUEUE_DEPTH, ring, 0);
	if (res != 0)
	{
		return errno;
	}

	res = create_threads(thread_function, server);
	if (res != 0)
	{
		io_uring_queue_exit(ring);
		return res;
	}

	res = prepare_read(ring, server->socket);
	if (res != 0)
	{
		io_uring_queue_exit(ring);
		return res;
	}

	while (1)
	{
		res = io_uring_wait_cqe(ring, &cqe);
		if (res != 0)
		{
			io_uring_queue_exit(ring);
			return res;
		}

		res = signal_ready_thread();
		if (res != 0)
		{
			continue;
		}

		res = wait_for_thread_signal();
		if (res != 0)
		{
			io_uring_queue_exit(ring);
			return res;
		}
	}
}

void *thread_function(Server *server)
{
	bool waiting_required = true;
	struct msghdr *message;
	int io_result, res, event_type;

	/* Infinite loop for continues packet handling */
	while (1)
	{
		/* fetches return data from Completion Queue and waits if necessary */
		int res = fetch_message(ring, cqe, &waiting_required, message, &io_result, &event_type);
		if (res != 0)
		{
			pthread_exit(&res);
		}

		switch (event_type)
		{
			case READ:
			{
				READ_START:

				res = resolve_read(server, message, io_result);
				switch (res)
				{
					case RESPOND:
					{
						res = prepare_write(ring, server->socket, message);
						if (res == SQ_FULL)
						{
							io_uring_submit(ring);
							goto READ_START;
						}
						break;
					}
					case RETRY:
					{
						goto READ_START;
					}
					case DROP:
					{
						return_buffer(message);
						break;
					}
					case WAIT:
					{
						sleep(1);
						break;
					}
				}

				res = prepare_read(ring, server->socket);
				if (res == SQ_FULL || res == NO_FREE_BUF)
				{
					if (res == SQ_FULL)
					{
						io_uring_submit(ring);
					}
					goto READ_START;
				}

				break;
			}
			case WRITE:
			{
				WRITE_START:

				res = resolve_write(server, message, io_result);
				switch (res)
				{
					case RETRY:
					{
						goto WRITE_START;
					}
					case DROP:
					{
						return_buffer(message);
						break;
					}
					case WAIT:
					{
						sleep(1);
						break;
					}
				}

				break;
			}
			default:
			{
				perror("Unexpected message type");
				break;
			}
		}

		res = finish_task(&waiting_required);
		if (res != 0)
		{
			pthread_exit(&res);
		}
	}
}