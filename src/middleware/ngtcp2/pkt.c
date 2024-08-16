//
// Created by moritz on 16.08.24.
//


int ngtcp2_conn_read_pkt_versioned(ngtcp2_conn *conn, const ngtcp2_path *path,
                                   int pkt_info_version,
                                   const ngtcp2_pkt_info *pi,
                                   const uint8_t *pkt, size_t pktlen,
                                   ngtcp2_tstamp ts)
{
	int rv = 0;
	ngtcp2_ssize nread = 0;
	const ngtcp2_pkt_info zero_pi =
		{0};
	(void)pkt_info_version;

	assert(!(conn->flags & NGTCP2_CONN_FLAG_PPE_PENDING));

	conn->log.last_ts = ts;
	conn->qlog.last_ts = ts;

	ngtcp2_log_info(&conn->log, NGTCP2_LOG_EVENT_CON, "recv packet len=%zu", pktlen);

	if (pktlen == 0)
	{
		return 0;
	}

	if (!pi)
	{
		pi = &zero_pi;
	}

	switch (conn->state)
	{
		case NGTCP2_CS_SERVER_INITIAL:
		case NGTCP2_CS_SERVER_WAIT_HANDSHAKE:
			if (!ngtcp2_path_eq(&conn->dcid.current.ps.path, path))
			{
				ngtcp2_log_info(&conn->log, NGTCP2_LOG_EVENT_CON, "ignore packet from unknown path during handshake");

				if (conn->state == NGTCP2_CS_SERVER_INITIAL &&
				    ngtcp2_strm_rx_offset(&conn->in_pktns->crypto.strm) == 0 &&
				    (!conn->in_pktns->crypto.strm.rx.rob ||
				     !ngtcp2_rob_data_buffered(conn->in_pktns->crypto.strm.rx.rob)))
				{
					return NGTCP2_ERR_DROP_CONN;
				}

				return 0;
			}

			nread = conn_read_handshake(conn, path, pi, pkt, pktlen, ts);
			if (nread < 0)
			{
				return (int)nread;
			}

			if ((size_t)nread == pktlen)
			{
				return 0;
			}

			assert(conn->pktns.crypto.rx.ckm);

			pkt += nread;
			pktlen -= (size_t)nread;

			break;
		case NGTCP2_CS_CLOSING:
			return NGTCP2_ERR_CLOSING;
		case NGTCP2_CS_DRAINING:
			return NGTCP2_ERR_DRAINING;
		case NGTCP2_CS_POST_HANDSHAKE:
			rv = conn_prepare_key_update(conn, ts);
			if (rv != 0)
			{
				return rv;
			}
			break;
		default:
			ngtcp2_unreachable();
	}

	return conn_recv_cpkt(conn, path, pi, pkt, pktlen, ts);
}