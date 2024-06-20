// Copyright (C) 2024 Moritz Scheer

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

struct connection
{
	/* Unique identifier for the connection, also used as hash key */
	ngtcp2_cid *cid;

	/* SSL session associated with the connection */
	SSL session;

	/* Path information for the connection */
	ngtcp2_path path;

	/* Pointer to the QUIC connection object */
	ngtcp2_conn *conn;

	/* Timer file descriptor for managing timeouts */
	int timer_fd;

	/* Flag indicating if a connection is being worked on */
	bool blocked;

	/* Hash table of active streams associated with the connection */
	Stream *streams;

	/* Hash function in use */
	UT_hash_handle hh;
};

/*
 *
 */
struct connection *create_connection(SSL session, int socket_fd);

/*
 *
 */
void close_connection();

/*
 *
 */
void free_connection();

/*
 *
 */
int read_connection();

/*
 *
 */
int write_connection();

/*
 *
 */
struct connection *find_connection(struct connection *connections, uint8_t *dcid, size_t dcid_size);

#endif //_CONNECTION_H_


