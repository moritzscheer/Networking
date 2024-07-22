// Copyright (C) 2024 Moritz Scheer

#ifndef _READ_QUEUE_H_
#define _READ_QUEUE_H_

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

static struct rqe
{
	uint32_t version;

	ngtcp2_cid scid;                    /* source connection id from client */

	uint8_t *iov_base;                  /* pointer to packet payload */

	size_t iov_len;                     /* length of packet payload */

	ngtcp2_tstamp timestamp;

	struct rqe *prev_recv_entry;        /* Pointer to last pkt received by the server */

	struct rqe *next;                   /* Pointer to next pkt from this Connection */
};

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

static struct io_uring_recvmsg_out *msg_out;

static pthread_mutex_t mutex;

static pthread_cond_t *cond;

static struct rqe *prev_entry;

static struct queue *available_queue;

static struct queue *blocked_queue;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int rq_init(void);

void rq_destroy(void);

inline struct rqe *create_rqe(uint32_t version, const uint8_t *scid, size_t scidlen, void *iov_base, size_t iov_len);

inline int enqueue_rqe(struct connection *connection, struct rqe *entry);

inline int dequeue_rqe(struct connection *connection, struct rqe *entry);

inline void finish_rqe(struct connection *connection, struct rqe *entry);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_READ_QUEUE_H_