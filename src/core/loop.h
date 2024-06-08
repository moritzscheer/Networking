// Copyright (C) 2024 Moritz Scheer

#ifndef _LOOP_H_
#define _LOOP_H_

#define QUEUE_DEPTH 60

void server_loop(server *server);

typedef void *(*WorkerFunction)(Server *server);

static int fetch_message(io_uring *ring, io_uring_cqe *cqe, msghdr *message, int *io_result);

static int finish_message();

#endif //_LOOP_H_
