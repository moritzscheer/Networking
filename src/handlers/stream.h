// Copyright (C) 2024, Moritz Scheer

#ifndef _STREAM_H_
#define _STREAM_H_

typedef struct Stream
{
	int64_t id;
	Queue *data;
	bool closed;
	/* invariant: sent_offset >= acked_offset */
	size_t sent_offset;
	size_t acked_offset;
};

typedef struct Queue
{
	struct iovec *head;
	struct iovec *tail;
} Queue;

Stream *create_stream(int64_t id);
void close_stream();
int stream_push_data();
void stream_finish_data();

#endif //_STREAM_H_