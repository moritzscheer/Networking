// Copyright (C) 2024 Moritz Scheer

#include <pthread.h>
#include <linux/types.h>
#include <linux/io_uring.h>
#include <sys/socket.h>

#include "threads.h"
#include "loop.h"
#include "../includes/status.h"
#include "../includes/server.h"

int signal_ready_thread()
{
	task_ready = true;
	return pthread_cond_signal(&cond_task_is_available);
}

void signal_cq_empty()
{
	cq_empty = true;
	return pthread_cond_signal(&cond_queue_done);
}

int wait_for_thread_signal()
{
	int res = 0;
	pthread_mutex_lock(&mutex);
	while (!cq_empty)
	{
		res = pthread_cond_wait(&cond_queue_done, &mutex);
	}
	cq_empty = false;
	pthread_mutex_unlock(&mutex);
	return res;
}

int fetch_message(io_uring *ring, io_uring_cqe *cqe, bool *waiting_required, msghdr *message, int *io_result)
{
	pthread_mutex_lock(&mutex);

	/* If the current thread is the only one ready, it can skip waiting for a task and start working on it directly */
	if (*waiting_required)
	{
		while(!task_ready)
		{
			pthread_cond_wait(&cond_task_is_available, &mutex);
		}
		task_ready = false;
	}

	threads_ready--;
	message = (struct msghdr *) cqe->user_data;
	*io_result = cqe->res;
	io_uring_cqe_seen(ring, cqe);

	/* if a thread and a task is available, send a signal to one other thread to start working on this task. */
	if (io_uring_peek_cqe(ring, &cqe) != -EAGAIN && threads_ready > 1)
	{
		int res = signal_ready_thread();
		if (res != 0)
		{
			return res;
		}
	}

	pthread_mutex_unlock(&mutex);
	return 0;
}

int finish_message(bool *waiting_required)
{
	int res = 0;
	pthread_mutex_lock(&mutex);

	threads_ready++;
	switch (io_uring_peek_cqe(ring, &cqe))
	{
		case -EAGAIN: /* no remaining work in Completion Queue. */
			if (threads_ready == 1)
			{
				res = signal_cq_empty();
			}
			*waiting_required = true;
			break;
		default:
			if (threads_ready == 1)
			{
				*waiting_required = false;
			}
			else
			{
				res = signal_ready_thread();
				*waiting_required = true;
			}
			break;
	}

	pthread_mutex_unlock(&mutex);
	return res;
}

int create_threads(ThreadFunction thread_function, Server *server)
{
	int res = 0;

	for (int I = 0; I < NUM_THREADS; I++)
	{
		res = pthread_create(server->threads[I], NULL, thread_function, server);
		if (res != 0)
		{
			for (int II = 0; II < I; II++)
			{
				free(server->threads[II])
			}
			return res;
		}
	}

	res = pthread_mutex_init(&mutex);
	if(res != 0)
	{
		return res;
	}
	res = pthread_cond_init(&cond_task_is_available);
	if(res != 0)
	{
		return res;
	}
	res = pthread_cond_init(&cond_queue_done);
	if(res != 0)
	{
		return res;
	}
}

int cleanup_threads(pthread threads[])
{
	int res = 0;

	for (int I = 0; I < NUM_THREADS; I++)
	{
		res = pthread_join(threads[I], NULL);
		if(res != 0)
		{
			return res;
		}
	}
	res = pthread_mutex_destroy(&mutex);
	if(res != 0)
	{
		return res;
	}
	res = pthread_cond_destroy(&cond_task_is_available);
	if(res != 0)
	{
		return res;
	}
	res = pthread_cond_destroy(&cond_queue_done);
	if(res != 0)
	{
		return res;
	}
	return res;
}