// Copyright (C) 2024 Moritz Scheer

#include "header.h"

static int decode_header(uint8_t *pkt, size_t pktlen, struct sockaddr_storage *addr, socklen_t addrlen)
{
	struct read_storage *event = NULL;

	if (pkt[0] & 0x80)
	{
		res = decode_long_header(event, pkt, pktlen, addr, addrlen);
		if (!event)
		{
			return res;
		}

		pthread_mutex_lock(&conn_mutex);

		res = get_connection(connection, &header.dcid);
		if (!connection)
		{
			pthread_mutex_unlock(&conn_mutex);
			return ENOMEM;
		}
	}
	else
	{
		res = decode_short_header(event, pkt, pktlen, addr, addrlen);
		if (!event)
		{
			return res;
		}

		pthread_mutex_lock(&conn_mutex);

		connection = find_connection(&event->dcid);
		if (!connection)
		{
			pthread_mutex_unlock(&conn_mutex);
			return send_stateless_reset_packet(&header.dcid, addr, addrlen);
		}
	}

	enqueue_read_event(connection, event);
	pthread_mutex_unlock(&conn_mutex);
	return res;
}

static int decode_short_header(struct read_storage *event, uint8_t *pkt, size_t pktlen, struct sockaddr_storage *addr,
                               socklen_t addrlen)
{
	if (pktlen < SHORT_HEADER_PKT_MIN_LENGTH)
	{
		return 0; // pkt size must be at least 1 byte + max cid length
	}

	if (!(pkt[0] & FIXED_BIT_MASK))
	{
		return 0; // clear fixed bit not allowed
	}

	event = calloc(1, sizeof(struct read_storage));
	if (!event)
	{
		return ENOMEM;
	}

	event->type = NGTCP2_PKT_1RTT;
	memcpy(event->dcid.data, pkt[1], NGTCP2_MAX_CIDLEN);
	event->dcid.datalen = NGTCP2_MAX_CIDLEN;
	event->receive_time = get_timestamp_ns();
	event->iov_base = iov_base;
	event->iov_len = iov_len;
	event->prev_recv_pkt = prev_read_event;
	return 0;
}

static int decode_long_header(struct read_storage *event, uint8_t *pkt, size_t pktlen, struct sockaddr_storage *addr,
                              socklen_t addrlen)
{
	if (pktlen < LONG_HEADER_PKT_MIN_LENGTH)
	{
		return 0; // pkt size must be at least 7 byte
	}

	uint32_t version = get_version(pkt);
	if (version == 0)
	{
		return handle_version_negotiation_pkt();
	}

	switch (pkt[0] & TYPE_BIT_MASK)
	{
		case PKT_INITIAL:
		{
			return handle_initial_pkt();
		}
		case PKT_0RTT:
		{
			return handle_0rtt_pkt();
		}
		case PKT_HANDSHAKE:
		{
			return handle_handshake_pkt();
		}
		case PKT_RETRY:
		{
			return handle_retry_pkt();
		}
		default:
		{
			return 0;
		}
	}

	size_t dcidlen = pkt[5];
	len += dcidlen;
	if (pktlen < len)
	{
		return 0;
	}
	ngtcp2_cid dcid;
	memcpy(&dcid, pkt[6], dcidlen);

	size_t scidlen = pkt[6 + dcidlen];
	len += scidlen;
	if (pktlen < len)
	{
		return 0;
	}
	ngtcp2_cid scid;
	memcpy(&dcid, pkt[6], scidlen);
}

uint32_t get_version(const uint8_t *pkt)
{
	uint32_t version = ((uint32_t) pkt[1] << 24) |
	                   ((uint32_t) pkt[2] << 16) |
	                   ((uint32_t) pkt[3] << 8) |
	                   (uint32_t) pkt[4];
	return ntohl(version);
}