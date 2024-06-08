// Copyright (C) 2024, Moritz Scheer

#include <pthread.h>
#include <linux/types.h>
#include <linux/io_uring.h>

#include "loop.h"
#include "threads.h"
#include "../handlers/read.h"
#include "../handlers/write.h"
#include "../includes/server.h"

pthread_cond_t cond_cqe_available;
bool cqe_available = false;

pthread_cond_t cond_cq_empty;
bool cq_empty = false;

void server_loop(Server *server)
{
	res = prepare_read(server->ring, server->socket, FIRST_ATTEMPT);
	if (res != 0)
	{
		io_uring_queue_exit(server->ring);
		return res;
	}

	while (1)
	{
		res = io_uring_wait_cqe(server->ring, &cqe);
		if (res != 0)
		{
			break;
		}

		pthread_mutex_lock(&mutex);
		res = broadcast_signal(&cond_cqe_available);
		if (res != 0)
		{
			break;
		}

		res = wait_for_signal(&cq_empty, &cond_cq_empty);
		if (res != 0)
		{
			break;
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_mutex_unlock(&mutex);

	io_uring_queue_exit(server->ring);
	return res;
}

int *worker_function(Server *server)
{
	int status_code;            /* local status variable */
	struct msghdr *message;     /* struct containing all packet data */
	int result;                 /* io operation result variable */
	uint8_t event_type, tries;  /* ancillary data saved in msghdr */

	while (1)
	{
		status_code = fetch_message(server->ring, server->cqe, message, &result, &event_type, &tries);
		if (status_code != 0)
		{
			return status_code;
		}

		switch (event_type)
		{
			case READ:
				status_code = resolve_read(server->socket, message, result);

				switch (status_code)
				{
					case RESPOND:
						status_code = prepare_write(server->ring, server->socket, message);
						if (status_code != 0)
						{
							return status_code;
						}
						break;
					case DROP:
						status_code = return_buffer(message);
						if (status_code != 0)
						{
							return status_code;
						}
						break;
					default:
						return status_code;
				}

				status_code = prepare_read(server->ring, server->socket);
				if (status_code != 0)
				{
					return status_code;
				}
				break;
			case WRITE:
				status_code = resolve_write(server, message, result);

				switch (status_code)
				{
					case DROP:
						status_code = return_buffer(message);
						if (status_code != 0)
						{
							return status_code;
						}
						break;
					case RETRY:
						status_code = prepare_write(server->ring, server->socket, message);
						if (status_code != 0)
						{
							return status_code;
						}
						break;
					default:
						return status_code;
				}
				break;
			default:
				perror("Unexpected message type");
				break;
		}

		status_code = finish_task();
		if (status_code != 0)
		{
			return status_code;
		}
	}
}

static int fetch_message(io_uring *ring, io_uring_cqe *cqe,
                         msghdr *message, int *io_result,
                         uint8_t event_type, uint8_t tries)
{
	int res;
	pthread_mutex_lock(&mutex);

	/* wait until an event is available */
	while (io_uring_peek_cqe(ring, &cqe) == -EAGAIN)
	{
		res = pthread_cond_wait(&cond_cqe_available, &mutex);
		if (res != 0)
		{
			pthread_mutex_unlock(&mutex);
			return res;
		}
	}

	threads_ready--;
	message = (struct msghdr *) cqe->user_data;
	io_result = cqe->res;
	io_uring_cqe_seen(ring, cqe);

	/* if an event is available, signal other threads */
	if (io_uring_peek_cqe(ring, &cqe) != -EAGAIN)
	{
		res = broadcast_signal(&cqe_available, &cond_cqe_available);
		if (res != 0)
		{
			pthread_mutex_unlock(&mutex);
			return res;
		}
	}

	pthread_mutex_unlock(&mutex);
	return 0;
}

static int finish_message()
{
	int res = 0;
	pthread_mutex_lock(&mutex);

	threads_ready++;
	if (io_uring_peek_cqe(ring, &cqe) == -EAGAIN)
	{
		cq_empty = !cq_empty;
		res = send_signal(&cond_cq_empty);
		if (res != 0)
		{
			pthread_mutex_unlock(&mutex);
			return res;
		}
	}

	pthread_mutex_unlock(&mutex);
	return res;
}