// Copyright (C) 2024 Moritz Scheer

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "parser.h"
#include "../includes/io_buffer.h"
#include "../includes/errno2.h"
#include "../handlers/connection.h"

int parse(Connection *connections, io_buffer *io_buffer)
{
	for (int I = 0; I < io_buffer->iovec_count; I++)
	{
		uint32_t version;
		const uint8_t *scid, *dcid;
		size_t dcidlen, scidlen;

		int res = decode(io_buffer-&gt;iovec[i], &version, &dcid, &dcidlen, &scid, &scidlen);
		if (res != 0)
			return res;

		Connection *connection = find_connection(connections, dcid, dcidlen);
		if (!connection)
		{
			connection = create_connection();
			if (!connection)
			{
				return -1;
			}
		}

		ngtcp2_path path;
		memcpy(&path, ngtcp2_conn_get_path(connection->conn), sizeof(path));
		path.remote.addrlen = remote_addrlen;
		path.remote.addr = (struct sockaddr*) &remote_addr;

		ngtcp2_pkt_info pi;
		memset(&pi, 0, sizeof(pi));

		ret = ngtcp2_conn_read_pkt (conn, &path, &pi, buf, n_read, timestamp ());
		if (ret == NGTCP_ERR_RETRY)
		{

		}
		else if (ret == NGTCP_ERR_DROP_CONN)
		{
			HASH_DEL(connections, connection);
			free_connection(connection);
		}
		else if (ret == NGTCP_ERR_DRAINING)
		{

		}
		else if (ret == NGTCP_ERR_CRYPTO)
		{

		}
		else
		{
			ngtcp2_conn_write_connection_close();
		}
	}
    return 0;
}

int decode(struct iovec iov, uint32_t *version, const uint8_t **dcid, size_t *dcidlen, const uint8_t **scid, size_t *scidlen)
	{
	int ret = ngtcp2_pkt_decode_version_cid(version,
	                                        dcid, dcidlen,
	                                        scid, scidlen,
	                                        iov.iov_base, iov.iov_len,
	                                        NGTCP2_MAX_CIDLEN);
	if(ret == NGTCP2_ERR_INVALID_ARGUMENT)
		return -1;
    else if(ret == NGTCP2_ERR_VERSION_NEGOTIATION)
		return ngtcp2_pkt_write_version_negotiation();
	else return 0;
}