// Copyright (C) 2024 Moritz Scheer

#ifndef _WORKER_H_
#define _WORKER_H_

#define QUEUE_DEPTH 60

/*
 *
 */
void server_loop(server *server);

/*
 *
 */
typedef void *(*WorkerFunction)(struct server *server);

#endif //_WORKER_H_
