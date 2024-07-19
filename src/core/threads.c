// Copyright (C) 2024 Moritz Scheer

#include <pthread.h>
#include <linux/types.h>
#include <linux/io_uring.h>
#include <sys/socket.h>
#include <stdlib.h>

#include "threads.h"
#include "worker.h"
#include "../includes/server.h"
#include "../includes/status.h"

int initialize_threads()
{
	for (int I = 0; I < NUM_THREADS; I++)
	{
		res = pthread_create(server->threads[I], NULL, worker_function, server);
		if (res != 0)
		{
			for (int II = 0; II < I; II++)
			{
				free(server->threads[II]);
			}
			return res;
		}
	}

	res = pthread_mutex_init(&mutex);
	if (res != 0)
	{
		return res;
	}

	return res;
}

int cleanup_threads()
{
	for (int I = 0; I < NUM_THREADS; I++)
	{
		res = pthread_join(threads[I], NULL);
		if (res != 0)
		{
			return res;
		}
	}
	res = pthread_mutex_destroy(&mutex);
	if (res != 0)
	{
		return res;
	}

	return res;
}

int send_signal(pthread_cond_t *cond_var)
{
	uint8_t tries = 0;
	do
	{
		res = pthread_cond_signal(cond_var);
		if (res == 0)
		{
			return 0;
		}
		tries++;
	}
	while (tries < 5);
	return res;
}

int broadcast_signal(pthread_cond_t *cond_var)
{
	uint8_t tries = 0;
	do
	{
		res = pthread_cond_broadcast(cond_var);
		if (res == 0)
		{
			return 0;
		}
		tries++;
	}
	while (tries < 5);
	return res;
}

int wait_for_signal(bool *cond, pthread_cond_t *cond_var)
{
	while (!(*cond))
	{
		res = pthread_cond_wait(cond_var, &mutex);
		if (res != 0)
		{
			pthread_mutex_unlock(&mutex);
			return res;
		}
	}
	*cond = !(*cond);
	return res;
}

