// Copyright (C) 2024 Moritz Scheer

#ifndef _SERVER_H_
#define _SERVER_H_

typedef struct Server
{
	/* Socket descriptor for the server */
	int socket;

	/* Local address information */
	struct sockaddr_storage local_addr;
	size_t local_addrlen;

	/* Array of worker threads */
	pthread_t threads[NUM_THREADS];

	/* I/O ring for asynchronous I/O operations */
	struct io_uring *ring;
	struct io_uring_cqe *cqe;

	/* QUIC settings and connection ID (CID) */
	ngtcp2_settings *settings;
	ngtcp2_cid *scid;

} Server;

#endif //_SERVER_H_