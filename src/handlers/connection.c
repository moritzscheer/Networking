// Copyright (C) 2024 Moritz Scheer

#include <string.h>
#include <ngtcp2/ngtcp2.h>
#include "connection.h"
#include "../middleware/ngtcp2/ngtcp2.h"
#include "../middleware/ngtcp2/packet.h"
#include "../includes/status.h"

struct connection *find_connection(ngtcp2_cid dcid)
{
	struct connection *connection;
	HASH_FIND(hh, connections, dcid, sizeof(dcid), connection);
	return connection;
}

int get_connection(struct connection *connection, ngtcp2_cid *dcid, ngtcp2_cid *scid, uint32_t *version,
                   struct sockaddr_storage *addr)
{
	HASH_FIND(hh, connections, dcid, sizeof(dcid), connection);
	if (!connection)
	{
		ngtcp2_cid *odcid;
		ngtcp2_token_type token_type;
		if(!initial_packet_is_valid())
		{
			return DROP_PACKET;
		}

		connection = calloc(1, sizeof(struct connection));
		if(!connection)
		{
			return ENOMEM;
		}

		res = create_ngtcp2_conn(connection, dcid, scid, version, addr, odcid, token_type);
		if(!connection->ngtcp2_conn)
		{
			free(connection);
			return res;
		}

		connection->cid = dcid;
		connection->streams = NULL;

		HASH_ADD_KEYPTR(hh, connections, dcid, dcidlen, connection);
		return 0;
	}
	else if (!scid_is_valid(connection->ngtcp2_conn, entry->scid))
	{
		return DROP_PACKET;
	}
}

int get_connection(struct connection *connection, ngtcp2_cid *dcid, ngtcp2_cid *scid, uint32_t *version,
                   struct sockaddr_storage *addr)
{
	struct connection *conn = NULL;

	HASH_FIND(hh, connections, dcid, sizeof(*dcid), conn);
	if (conn)
	{
		if (!scid_valid(conn->ngtcp2_conn, scid))
		{
			return NGTCP2_ERR_DROP;
		}
		*connection = conn;
		return 0;
	}

	ngtcp2_token_type token_type = 0;
	ngtcp2_cid *original_dcid = NULL;
	ngtcp2_path path;

	res = verify_initial_packet(dcid, version, addr, token_type, original_dcid);
	if (res != 0)
	{
		return res;
	}

	conn = calloc(1, sizeof(struct connection));
	if (!conn)
	{
		return ENOMEM;
	}

	res = create_ngtcp2_conn(conn, dcid, scid, version, addr, token_type, original_dcid);
	if (!conn->ngtcp2_conn)
	{
		free(conn);
		return res;
	}

	conn->cid = dcid;
	conn->streams = NULL;

	HASH_ADD_KEYPTR(hh, connections, conn->cid, sizeof(*dcid), conn);

	*connection = conn;
	return 0;
}

int create_connection(struct connection *connection, ngtcp2_cid *dcid, ngtcp2_cid *scid, void *pkt, size_t pktlen,
                      struct sockaddr_storage *addr)
{
	if(check_initial_packet())
	{
		return 0;
	}

	struct connection connection = calloc(1, sizeof(struct connection));
	if(!connection)
	{
		return ENOMEM;
	}

	create_ngtcp2_conn(cid);

	connection->cid = cid
	connection->streams = NULL;

	HASH_ADD_KEYPTR(hh, connections, dcid, dcidlen, connection);
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