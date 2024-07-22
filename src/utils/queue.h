//
// Created by morit on 19.07.2024.
//

#ifndef _QUEUE_H_
#define _QUEUE_H_

struct queue
{
	void *head;
	void *tail;
};

bool is_empty(struct queue *queue)
{
	return queue->head == NULL;
}

void enqueue(struct queue *queue, void *entry)
{
	if (!connection->head)
	{
		queue->head = entry;
		queue->tail = entry;
	}
	else
	{
		queue->tail->next = entry;
		queue->tail = tail->next;
	}
}

int enqueue_and_signal(struct queue *queue, void *entry, pthread_cond_t *cond)
{
	if (!connection->head)
	{
		queue->head = entry;
		queue->tail = entry;
		return send_signal(&cond);
	}
	queue->tail->next = entry;
	queue->tail = tail->next;
	return 0;
}

void *dequeue(struct queue *queue)
{
	if (queue->head == NULL)
	{
		return NULL;
	}

	void *entry = queue->head;
	queue->head = queue->head->next;

	if (queue->head == NULL)
	{
		queue->tail = NULL;
	}

	return entry;
}

int dequeue_or_wait(struct queue *queue, void *entry, pthread_cond_t *cond)
{
	while (IsEmpty(available_queue))
	{
		res = pthread_cond_wait(&cond, mutex);
		if (res != 0)
		{
			return res;
		}
	}

	entry = queue->head;
	queue->head = queue->head->next;

	if (queue->head == NULL)
	{
		queue->tail = NULL;
	}
	return 0;
}

void *peek(struct queue *queue)
{
	if (queue->head == NULL)
	{
		return NULL;
	}
	return queue->head;
}

int size(struct queue *queue)
{
	int size = 0;
	void *current = queue->head;
	while (current != NULL)
	{
		size++;
		current = current->next;
	}
	return size;
}

void free_queue(struct queue *queue)
{
	struct rqe *current = queue->head;
	struct rqe *next;

	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}
	free(queue);
}

#endif //_QUEUE_H_
