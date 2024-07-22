// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "write_queue.h"
#include "../../utils/utils.h"

int wq_init(void)
{
	available_queue = calloc(1, sizeof(struct queue) * 2);
	if (!available_queue)
	{
		return NOMEM;
	}

	res = pthread_mutex_init(&mutex);
	if (res != 0)
	{
		return res;
	}

	res = pthread_cond_init(&cond, NULL);
	if (res != 0)
	{
		return res;
	}

	return 0;
}

int wq_destroy(void)
{
	free_queue();

	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
}

struct wqe *create_wqe()
{
	struct wqe *entry = malloc(sizeof(struct wqe));
	if (!entry)
	{
		return NULL;
	}

	return entry;
}

int enqueue_wqe()
{
	pthread_mutex_lock(&mutex);

	pthread_mutex_unlock(&mutex);
	return 0;
}

int dequeue_wqe()
{
	pthread_mutex_lock(&mutex);

	pthread_mutex_unlock(&mutex);
	return 0;
}

void finish_wqe()
{
	pthread_mutex_lock(&mutex);

	pthread_mutex_unlock(&mutex);
	free(entry);
}