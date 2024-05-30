// Copyright (C) 2024 Moritz Scheer

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

typedef struct
{
	int64_t id;
	SSL session;
	ngtcp2_conn *conn;
	int socket_fd;
	int timer_fd;
	Stream *streams;
	struct sockaddr_storage local_addr;
	size_t local_addrlen;
	struct sockaddr_storage remote_addr;
	size_t remote_addrlen;
	UT_hash_handle hh;
} Connection;

Connection create_connection(SSL session, int socket_fd);
void close_connection();
void free_connection();
int read_connection();
int write connection();
Connection *find_connection(Connection* connections, uint8_t *dcid, size_t dcid_size);

#endif //_CONNECTION_H_
