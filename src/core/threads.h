// Copyright (C) 2024 Moritz Scheer

#ifndef _THREADS_H_
#define _THREADS_H_

pthread_mutex_t mutex;

int threads_ready = MAX_THREADS;

int initialize_threads(Server *server);

int cleanup_threads();

int send_signal(pthread_cond_t *cond_var);

int broadcast_signal(pthread_cond_t *cond_var);

int wait_for_signal(bool *cond, pthread_cond_t *cond_var);

#endif //_THREADS_H_