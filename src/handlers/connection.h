// Copyright (C) 2024 Moritz Scheer

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

typedef struct Connection
{
	/* Unique identifier for the connection */
	ngtcp2_cid *cid;

	/* SSL session associated with the connection */
	SSL session;

	/* Path information for the connection */
	ngtcp2_path path;

	/* Pointer to the QUIC connection object */
	ngtcp2_conn *conn;

	/* Timer file descriptor for managing timeouts */
	int timer_fd;

	/* Hash table of active streams associated with the connection */
	Stream *streams;
	UT_hash_handle hh;

} Connection;

Connection create_connection(SSL session, int socket_fd);
void close_connection();
void free_connection();
int read_connection();
int write
connection();
Connection *find_connection(Connection *connections, uint8_t *dcid, size_t dcid_size);

#endif //_CONNECTION_H_
