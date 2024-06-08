// Copyright (C) 2024, Moritz Scheer

int parse_packet(Connection *connections, struct msghdr *msghdr)
{
	for (int I = 0; I < msghdr->msg_iovlen; I++)
	{
		uint32_t version;
		const uint8_t *scid, *dcid;
		size_t dcidlen, scidlen;

		switch (decode_quic_header(version, dcid, dcidlen, scid, scidlen, iov.iov_base, iov.iov_len, NGTCP2_MAX_CIDLEN))
		{
			case 0:
			{
				break;
			}
			case NGTCP2_ERR_VERSION_NEGOTIATION:
			{
				get_version_negotiation_packet(vc.version, {vc.scid, vc.scidlen}, {vc.dcid, vc.dcidlen}, ep, local_addr, sa,
				                               salen);
				return;
			}
			default:
			{
				return;
			}
		}

		Connection *connection = find_connection(connections, dcid, dcidlen);
		if (!connection)
		{
			connection = create_connection();
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

		ret = ngtcp2_conn_read_pkt(conn, &path, &pi, buf, n_read, timestamp());
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

static int
decode_quic_header(struct iovec iov, uint32_t *version, const uint8_t **dcid, size_t *dcidlen, const uint8_t **scid, size_t *scidlen)
{
	int ret = ngtcp2_pkt_decode_version_cid(version,
	                                        dcid, dcidlen,
	                                        scid, scidlen,
	                                        iov.iov_base, iov.iov_len,
	                                        NGTCP2_MAX_CIDLEN);
	if (ret == NGTCP2_ERR_INVALID_ARGUMENT)
	{
		return -1;
	}
	else if (ret == NGTCP2_ERR_VERSION_NEGOTIATION)
	{
		return ngtcp2_pkt_write_version_negotiation();
	}
	return 0;
}