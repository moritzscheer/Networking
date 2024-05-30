// Copyright (C) 2024 Moritz Scheer

#ifndef _THREADS_H_
#define _THREADS_H_

pthread_mutex_t mutex;
pthread_cond_t cond_task_is_available;
pthread_cond_t cond_queue_done;

static int threads_ready = MAX_THREADS;

static bool task_ready = false;

static bool cq_empty = false;

int signal_ready_thread();

void signal_queue_done();

int wait_for_finished_threads();

int fetch_message(io_uring *ring, io_uring_cqe *cqe, bool *wait_necessary, msghdr *message);

int finish_message(bool *wait_necessary);

int create_threads(Server *server, ThreadFunction thread_function);

int cleanup_threads();

#endif //_THREADS_H_