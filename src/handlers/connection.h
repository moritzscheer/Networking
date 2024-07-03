// Copyright (C) 2024 Moritz Scheer

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */



/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

struct connection
{
	ngtcp2_cid *cid;            /* Unique identifier for the connection, also used as hash key */
	ngtcp2_conn *conn;          /* Pointer to the QUIC connection object */
	SSL_CTX *ssl_ctx;               /* SSL session associated with the connection */
	ngtcp2_path *path;          /* Path information for the connection */
	int timer_fd;
	Stream *streams;
	int num_streams;
	struct Connection *next;    /* pointer to either next available Connection or next blocked Connection */
	struct pkt *head;           /* pointers to head of queue for packets of this connection */
	struct pkt *tail;           /* pointers to tail of queue for packets of this connection */
	bool blocked;               /* Flag indicating if a connection is being worked on */
	UT_hash_handle hh;          /* Hash function in use */
};

struct conn_queue
{
	struct connection *head;
	struct connection *tail;
};

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */



/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

struct connection *create_connection(SSL session, int socket_fd);

void close_connection();

void free_connection();

int read_connection();

int write_connection();

struct connection *find_connection(struct connection *connections, uint8_t *dcid, size_t dcid_size);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_CONNECTION_H_


