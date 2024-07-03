// Copyright (C) 2024 Moritz Scheer

#ifndef _THREADS_H_
#define _THREADS_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */



/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */



/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

pthread_mutex_t mutex;

int threads_ready = MAX_THREADS;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int initialize_threads(struct server *server);

int cleanup_threads();

int send_signal(pthread_cond_t *cond_var);

int broadcast_signal(pthread_cond_t *cond_var);

int wait_for_signal(bool *cond, pthread_cond_t *cond_var);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_THREADS_H_