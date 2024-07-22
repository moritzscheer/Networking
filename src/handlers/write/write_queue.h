// Copyright (C) 2024 Moritz Scheer

#ifndef _WRITE_QUEUE_H_
#define _WRITE_QUEUE_H_

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

static struct rqe
{
	uint32_t version;

	ngtcp2_cid scid;                    /* source connection id from client */

	uint8_t *iov_base;                  /* pointer to packet payload */

	size_t iov_len;                     /* length of packet payload */

	ngtcp2_tstamp timestamp;

	struct pkt *prev_recv_pkt;          /* Pointer to last pkt received by the server */

	struct pkt *next;                   /* Pointer to next pkt from this Connection */
};

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

static pthread_mutex_t mutex;

static pthread_cond_t *cond;

static struct queue *queue;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int wq_init(void);

int wq_destroy(void);

struct wqe *create_read_pkt();

int enqueue_read_pkt();

int dequeue_read_pkt();

void finish_read_pkt();

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_WRITE_QUEUE_H_