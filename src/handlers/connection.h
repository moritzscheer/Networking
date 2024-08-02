// Copyright (C) 2024 Moritz Scheer

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */



/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

struct connection
{
	ngtcp2_cid cid;

	ngtcp2_conn *ngtcp2_conn;

	nghttp3_conn *nghttp3_conn;

	WOLFSSL_CTX *ssl_ctx;

	ngtcp2_path *path;

	int timer;

	bool blocked;

	struct stream *streams;

	int stream_count;

	struct read_pkt_queue packets;

	struct Connection *next;
};

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

static pthread_mutex_t conn_mutex;

static struct connection *connections = NULL;

static UT_hash_handle hh;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

struct connection *find_connection(ngtcp2_cid cid);

int create_connection(struct connection *connection, ngtcp2_cid *dcid, ngtcp2_cid *scid, void *pkt, size_t pktlen,
                      struct sockaddr_storage *addr);

void close_connection(struct connection *connection);

void free_connection(struct connection *connection);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_CONNECTION_H_


