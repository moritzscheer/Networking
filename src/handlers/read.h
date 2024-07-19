// Copyright (C) 2024 Moritz Scheer

#ifndef _READ_H_
#define _READ_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define READ 2

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

static struct pkt
{
	uint32_t version;

	ngtcp2_cid scid;                    /* source connection id from client */

	uint8_t *iov_base;                  /* pointer to packet payload */

	size_t iov_len;                     /* length of packet payload */

	struct pkt *prev_recv_task;         /* Pointer to last pkt received by the server */

	struct pkt *next;                   /* Pointer to next pkt from the Connection */
};

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

static struct io_uring_recvmsg_out *msg_out;

static pthread_mutex_t read_mutex;

static struct pkt *prev_pkt = NULL;

static struct conn_queue *available_queue;

static struct conn_queue *blocked_queue;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int prepare_read();

inline int validate_read(struct io_uring_cqe *cqe);

static inline int resolve_success(void *iov_base, size_t iov_len);

static inline int resolve_error(int result);

int dequeue_connection(struct connection *connection);

int enqueue_connection(struct connection *connection);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_READ_H_