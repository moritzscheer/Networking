// Copyright (C) 2024, Moritz Scheer

#include "response.h"

int parse_packet(Connection *connections, struct msghdr *msghdr)
{
	int res;

	for (int I = 0; I < msghdr->msg_iovlen; I++)
	{
		uint32_t version;
		const uint8_t *scid, *dcid;
		size_t dcidlen, scidlen;

		switch (ngtcp2_pkt_decode_version_cid(version, dcid, dcidlen, scid, scidlen, iov.iov_base, iov.iov_len,
		                                      NGTCP2_MAX_CIDLEN))
		{
			case 0:
			{
				break;
			}
			case NGTCP2_ERR_VERSION_NEGOTIATION:
			{
				return get_version_negotiation_packet(vc.version, {vc.scid, vc.scidlen}, {vc.dcid, vc.dcidlen}, ep,
				                                      local_addr, sa, salen);
			}
			default:
			{
				return DROP;
			}
		}

		Connection *connection = find_connection(connections, dcid, dcidlen);
		if (!connection)
		{
			connection = create_connection(connections);
			if (!connection)
			{
				return DROP;
			}
		}

		ngtcp2_path path;
		memcpy(&path, ngtcp2_conn_get_path(connection->conn), sizeof(path));
		path.remote.addrlen = remote_addrlen;
		path.remote.addr = (struct sockaddr *) &remote_addr;

		ngtcp2_pkt_info pi;
		memset(&pi, 0, sizeof(pi));

		switch (ngtcp2_conn_read_pkt(conn, &path, &pi, buf, n_read, timestamp()))
		{
			case 0:
			{
				return get_response_packet();
			}
			case NGTCP_ERR_RETRY:
			{
				return get_retry_packet();
			}
			case NGTCP_ERR_DROP_CONN:
			{
				return drop_connection();
			}
			case NGTCP_ERR_DRAINING:
			{
				return drain_connection();
			}
			case NGTCP_ERR_CRYPTO:
			{
				return alert_tls_stack();
			}
			default:
			{
				return close_connection();
			}
		}
	}
}