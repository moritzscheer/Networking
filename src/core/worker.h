// Copyright (C) 2024 Moritz Scheer

#ifndef _WORKER_H_
#define _WORKER_H_


/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define QUEUE_DEPTH 60

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */



/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */



/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

typedef int *(*WorkerFunction)(struct server *server);

int handle_quic_events(struct pkt *packet);

int handle_http_events(struct pkt *packet);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_WORKER_H_
