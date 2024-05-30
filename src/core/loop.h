// Copyright (C) 2024 Moritz Scheer

#ifndef _LOOP_H_
#define _LOOP_H_

#define QUEUE_DEPTH 60
#define MAIN_SERVER_LOOP 1
#define REQUESTS_IN_QUEUE 1

typedef void *(*ThreadFunction)(Server *server);

void server_loop(server *server)

#endif //_LOOP_H_
