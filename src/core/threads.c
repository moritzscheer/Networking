// Copyright (C) 2024 Moritz Scheer

#include <pthread.h>
#include <stdatomic.h>
#include <linux/types.h>
#include <linux/io_uring.h>

#include "../includes/status.h"

pthread_mutex_t mutex;
pthread_cond_t thread_cond;
pthread_cond_t queue_cond;

static int ready_threads = MAX_THREADS;
static bool error_occurred = false;
static bool queue_done = false;

void signal_queue_done()
{
	// sends custom signal
}

void signal_new_task()
{
	// sends custom signal
}

int wait_for_response()
{
	while (!queue_done && !error_occurred)
	{
		pthread_cond_wait(&queue_cond, &mutex);
	}
	return error_occurred ? THREAD_ERR : 0;
}

void wait_for_new_task()
{
	while (!tasks_open)
		pthread_cond_wait(&thread_cond, &mutex);
}

int start_handling_tasks()
{
	pthread_mutex_lock(&mutex);

	int result = signal_new_task();
	if (result == 0)
	{
		result = wait_for_response();
	}

	pthread_mutex_unlock(&mutex);
	return result;
}

int get_new_task(io_uring *ring, io_uring_cqe *cqe, bool *wait_is_necessary, msghdr *message)
{
	pthread_mutex_lock(&mutex);

	/* wait_is_necessary is set to false, when this thread can immediately start working on next task */
	if (*wait_is_necessary)
	{
		wait_for_new_task();
	}

	ready_threads--;

	/* gets entry from Completion Queue and marks it as seen */
	message = (struct msghdr *) cqe->user_data;
	io_uring_cqe_seen(ring, cqe);

	/* send a signal to other threads to start working on a task, if any thread and task is available */
	if (io_uring_peek_cqe(ring, &cqe) != -EAGAIN && ready_threads > 1)
	{
		int result = signal_new_task();
		if (result != 0)
		{
			return result;
		}
	}

	pthread_mutex_unlock(&mutex);
	return 0;
}

int finish_task(bool *wait_is_necessary)
{
	int result;
	pthread_mutex_lock(&mutex);
	ready_threads++;

	if (io_uring_peek_cqe(ring, &cqe) == -EAGAIN)
	{
		/* no remaining work in completion queue */
		result = signal_queue_done();
		if (result != 0)
			return result;
		*wait_is_necessary = true;
	}
	else if (ready_threads == 1)
	{
		/* this thread can directly start with the next task */
		*wait_is_necessary = false;
	}
	else
	{
		/* another thread can do the next task */
		result = signal_new_task();
		if (result != 0)
			return result;
		*wait_is_necessary = true;
	}

	pthread_mutex_unlock(&mutex);
	return 0;
}