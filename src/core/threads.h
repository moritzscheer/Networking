// Copyright (C) 2024 Moritz Scheer

#ifndef _THREADS_H_
#define _THREADS_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */



/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */



/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

pthread_mutex_t mutex;

int threads_ready = MAX_THREADS;

/* ------------------------------------------- TYPEDEF DECLARATIONS ------------------------------------------------- */

typedef int *(*initialize_threads)(void);

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int initialize_threads(void);

int cleanup_threads();

int send_signal(pthread_cond_t *cond_var);

int broadcast_signal(pthread_cond_t *cond_var);

int wait_for_signal(bool *cond, pthread_cond_t *cond_var);

inline int unlock_and_return(pthread_mutex_t *mutex, int res);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_THREADS_H_