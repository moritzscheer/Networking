// Copyright (C) 2024 Moritz Scheer

#include <string.h>
#include <ngtcp2/ngtcp2.h>
#include "connection.h"
#include "../handlers/connection.h"
#include "../middleware/ngtcp2/ngtcp2.h"
#include "../middleware/ngtcp2/packet.h"
#include "../includes/status.h"

struct connection *find_connection(ngtcp2_cid *dcid)
{
	struct connection *connection;
	HASH_FIND(hh, connections, event->dcid, sizeof(event->dcid), connection);
	return connection;
}

int get_connection(struct connection *connection, struct read_event *event)
{
	HASH_FIND(hh, connections, event->dcid, sizeof(event->dcid), connection);
	if (connection)
	{
		if (!scid_valid(connection->ngtcp2_conn, scid))
		{
			connection = NULL;
		}
		return 0;
	}

	ngtcp2_cid *original_dcid = NULL;
	ngtcp2_token_type token_type = NGTCP2_TOKEN_TYPE_UNKNOWN;

	res = verify_initial_packet(original_dcid, token_type, event);
	if (token_type == NGTCP2_TOKEN_TYPE_UNKNOWN)
	{
		return res;
	}

	connection = calloc(1, sizeof(struct connection));
	if (!connection)
	{
		return ENOMEM;
	}

	res = create_ngtcp2_conn(connection, event, token_type, original_dcid);
	if (!connection->ngtcp2_conn)
	{
		free(connection);
		return res;
	}

	connection->cid = event->dcid;
	conn->streams = NULL;

	HASH_ADD_KEYPTR(hh, connections, connection->cid, sizeof(connection->cid), connection);
	return 0;
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