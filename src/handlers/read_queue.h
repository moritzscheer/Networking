// Copyright (C) 2024 Moritz Scheer

#ifndef _READ_QUEUE_H_
#define _READ_QUEUE_H_

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

struct pkt
{
	ngtcp2_cid dcid;
	ngtcp2_cid scid;
	time_t receive_time;
	uint8_t iov_base;
	size_t iov_len;

	/* Pointer to next packet from the Connection */
	struct pkt *next;

	/* Pointer to last packet received by the server. can be a packet from any Connection */
	struct pkt *prev_recv_pkt;
};

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

pthread_mutex_t read_mutex;

static struct conn_queue *available_queue;

static struct conn_queue *blocked_queue;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

struct pkt *create_packet(struct io_uring_recvmsg_out *msg_out, struct pkt *prev_pkt, ngtcp2_version_cid vcid);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_READ_QUEUE_H_
