// Copyright (C) 2024 Moritz Scheer

#include "connection.h"

pthread_mutex_t conn_mutex;

struct connection *connections = NULL;

struct connection *create_connection(ngtcp2_cid *cid, struct sockaddr_union remote_addr, size_t remote_addrlen)
{
	struct connection connection = calloc(1, sizeof(struct connection));
	if(!connection)
	{
		return NULL;
	}

	setup_ngtcp2_conn(cid);

	connection->cid = cid
	connection->streams = NULL;
	return connection;
}

void close_connection()
{

}

void drop_connection()
{
	/*
	HASH_DEL(connections, connection);
	free_connection(connection);
	*/
}

int read_connection()
{

}

int write_connection()
{

}

struct connection *find_connection(ngtcp2_cid cid)
{
	struct connection *connection;
	pthread_mutex_lock(&conn_mutex);
	HASH_FIND(connection->hh, connections, &cid, sizeof(ngtcp2_cid), connection);
	pthread_mutex_unlock(&conn_mutex);
	return connection;
}

void connection_add_stream(struct connection *connection, Stream *stream)
{
	for(int I = 0; I < connection->streamslen; I++)
	{

	}
}

void connection_remove_stream(struct connection *connection, Stream *stream)
{

}

ngtcp2_cid *generate_cid()
{
	ngtcp2_cid *cid;
	uint8_t buf[NGTCP2_MAX_CIDLEN];

	if (RAND_bytes(buf, sizeof(buf)) != 1)
	{
		return NULL;
	}
	ngtcp2_cid_init(cid, buf, sizeof(buf));
	return cid;
}