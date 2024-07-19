// Copyright (C) 2024 Moritz Scheer

#include "connection.h"

struct connection *find_connection(ngtcp2_cid cid)
{
	struct connection *connection;
	pthread_mutex_lock(&conn_mutex);
	HASH_FIND(connection->hh, connections, &cid, sizeof(ngtcp2_cid), connection);
	pthread_mutex_unlock(&conn_mutex);
	return connection;
}

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

void close_connection(struct connection *connection)
{
	free_connection();
	send_connection_close_packet();
}

void free_connection(struct connection *connection)
{
	struct task *current = connection->head;
	struct task *next;
	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}

	HASH_DEL(connections, connection);
	free(connection);
}
