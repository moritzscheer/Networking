// Copyright (C) 2024 Moritz Scheer

#include "connection.h"

pthread_mutex_t conn_mutex;

struct connection *create_connection(SSL session, int socket_fd)
{
	ngtcp2_pkt_hd header;
	if (ngtcp2_accept(&header, data, data_size) < 0)
	{
		return NULL;
	}

	struct connection connection = (struct connection *) calloc(1, sizeof(struct connection));
	if(!connection)
	{
		return NULL;
	}

	connection->cid = generate_cid();
	if (!connection->cid)
	{
		return NULL;
	}

	connection->session = create_session();
	if (!connection->session)
	{
		return NULL;
	}

	connection->path =
		{
			.local = {
				.addrlen = server->local_addrlen,
				.addr = (struct sockaddr *) &server->local_addr
			},
			.remote = {
				.addrlen = remote_addrlen,
				.addr = (struct sockaddr *)remote_addr
			}
		};

	ngtcp2_transport_params params;
	ngtcp2_transport_params_default (&params);
	params.initial_max_streams_uni = 50;
	params.initial_max_streams_bidi = 100;
	params.initial_max_stream_data_bidi_local = 128 * 1024;
	params.initial_max_stream_data_bidi_remote = 128 * 1024;
	params.initial_max_data = 1024 * 1024;
	memcpy(&params.original_dcid, &header.dcid, sizeof (params.original_dcid));

	ngtcp2_conn *conn = NULL;
	if (ngtcp2_conn_server_new(&conn, &header.scid, &connection->cid, &path, header.version, &callbacks,
							   &server->settings, &params, NULL, connection) < 0)
	{
		return NULL;
	}

	connection->streams = NULL;
	return connection;
}

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

struct connection *find_connection(struct connection *connections, uint8_t *dcid, size_t dcid_size)
{
	struct connection *connection;

	for (connection = connections; connection != NULL; connection = connection->hh.next)
	{
		size_t n_scids = ngtcp2_conn_get_scid(connection->conn, NULL);

		ngtcp2_cid *scids = malloc(sizeof(ngtcp2_cid) * n_scids);
		if (!scids)
		{
			return NULL;
		}
		ngtcp2_conn_get_scid(connection->conn, scids);

		for (size_t i = 0; i < n_scids; i++)
		{
			if (dcid_size == scids[i].datalen && memcmp(dcid, scids[i].data, dcid_size) == 0)
			{
				free(scids);
				return connection;
			}
		}
		free(scids);
	}
	return NULL;
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