// Copyright (C) 2024, Moritz Scheer

#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "read_queue.h"
#include "../../core/threads.h"
#include "../../utils/queue.h"
#include "../../utils/utils.h"

int rq_init(void)
{
	available_queue = calloc(1, sizeof(struct queue) * 2);
	if (!available_queue)
	{
		return ENOMEM;
	}

	blocked_queue = calloc(1, sizeof(struct queue) * 2);
	if (!available_queue)
	{
		return ENOMEM;
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

	prev_pkt = NULL;
	return 0;
}

void rq_destroy(void)
{
	free_queue(availabe_queue);
	free_queue(blocked_queue);

	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
}

inline struct rqe *create_rqe(ngtcp2_version_cid *vcid, void *pkt, size_t pktlen, struct sockaddr_storage *addr)
{
	struct rqe *entry = calloc(1, sizeof(struct rqe));
	if (!entry)
	{
		return NULL;
	}

	ngtcp2_cid_init(&entry->dcid, vcid->dcid, vcid->dcidlen);

	if (vcid->scid)
	{
		ngtcp2_cid_init(&entry->scid, vcid->scid, vcid->scidlen);
	}

	entry->version = vcid->version;
	entry->scid = scid;
	entry->receive_time = get_timestamp_ns();
	entry->iov_base = iov_base;
	entry->iov_len = iov_len;
	entry->prev_recv_pkt = prev_entry;

	prev_entry = entry;
	return entry;
}

inline int enqueue_rqe(struct connection *connection, struct rqe *entry)
{
	pthread_mutex_lock(&mutex);

	if (is_empty(connection->packets))
	{
		res = enqueue_and_signal(available_queue, connection);
		if (res != 0)
		{
			return unlock_and_return(&mutex, res);
		}
	}
	enqueue(connection->packets, entry);

	return unlock_and_return(&mutex, 0);
}

inline int dequeue_rqe(struct connection *connection, struct rqe *entry)
{
	pthread_mutex_lock(&mutex);

	res = dequeue_or_wait(available_queue, connection, &cond);
	if (res != 0)
	{
		return unlock_and_return(&mutex, res);
	}

	enqueue(blocked_queue, connection);
	connection->blocked = true;

	entry = dequeue(connection->packets);

	return unlock_and_return(&mutex, 0);
}

inline void finish_rqe(struct connection *connection, struct rqe *entry)
{
	pthread_mutex_lock(&mutex);

	if (!is_empty(connection->packets))
	{

	}
	connection->blocked = false;

	pthread_mutex_unlock(&mutex);
	free(entry);
}