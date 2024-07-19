// Copyright (C) 2024 Moritz Scheer

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */



/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

struct connection
{
	// unique identifier cid for hash function hh
	ngtcp2_cid cid;
	UT_hash_handle hh;

	// connection objects for handling events
	ngtcp2_conn *ngtcp2_conn;
	nghttp3_conn *nghttp3_conn;
	SSL_CTX *ssl_ctx;
	ngtcp2_path *path;

	int timer;

	bool blocked;

	struct stream *streams;
	int stream_count;

	// received packets for this connection are stored as tasks in a queue
	struct task *head;
	struct task *tail;

	struct Connection *next;
};

struct conn_queue
{
	struct connection *head;
	struct connection *tail;
};

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

static pthread_mutex_t conn_mutex;

struct connection *connections = NULL;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

struct connection *find_connection(ngtcp2_cid cid);

struct connection *create_connection(ngtcp2_cid *cid, struct sockaddr_union remote_addr, size_t remote_addrlen);

void close_connection(struct connection *connection);

void free_connection(struct connection *connection);

int write_connection();

struct connection *find_connection(struct connection *connections, uint8_t *dcid, size_t dcid_size);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_CONNECTION_H_


