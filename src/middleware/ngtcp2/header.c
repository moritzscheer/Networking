// Copyright (C) 2024 Moritz Scheer

#include "h"
#include "../../utils/convert.h"

static int decode_short_header(struct read_storage *event, uint8_t *pkt, size_t pktlen, struct sockaddr_storage *addr,
                               socklen_t addrlen)
{
	if (pktlen < SHORT_HEADER_PKT_MIN_LENGTH) // pkt size must be at least 1 byte + max cid length
	{
		return 0;
	}

	if (!(pkt[0] & FIXED_BIT_MASK)) // clear fixed bit not allowed
	{
		return 0;
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
	size_t len = LONG_HEADER_PKT_MIN_LENGTH;

	if (pktlen < len || !(pkt[0] & FIXED_BIT_MASK))
	{
		return 0;
	}

	size_t dcidlen = pkt[5];
	len += dcidlen;
	if (pktlen < len)
	{
		return 0;
	}

	size_t scidlen = pkt[6 + dcidlen];
	len += scidlen;
	if (pktlen < len)
	{
		return 0;
	}

	uint32_t version = get_version(pkt);
	if (version == 0)
	{
		return send_version_negotiation_packet(&pkt[6], dcidlen, &pkt[6 + dcidlen + 1], scidlen);
	}

	ngtcp2_cid dcid;
	dcid.datalen = dcidlen;
	memcpy(&dcid, &pkt[6], dcidlen);

	ngtcp2_cid scid;
	dcid.datalen = scidlen;
	memcpy(&dcid, &pkt[6 + dcidlen + 1], scidlen);

	len += 1;
	if (pktlen < len)
	{
		return 0;
	}

	ngtcp2_token_type token_type = NGTCP2_TOKEN_TYPE_UNKNOWN;
	ngtcp2_cid *original_dcid = NULL;

	switch (pkt[0] & TYPE_BIT_MASK)
	{
		case PKT_INITIAL:
		{
			res = verify_token(odcid, token, original_dcid, &token_type);
			if (res != 0)
			{
				return res;
			}

			break;
		}
		case PKT_0RTT:
		{
			return 0;
		}
		default:
		{
			return 0;
		}
	}

	size_t rest_length = get_uvarintlen(&pkt[len]);
	len += rest_length - 1;
	if (pktlen < len)
	{
		return 0;
	}

	res = get_packet_number();
	if (res != 0)
	{
		return res;
	}
}

int verify_token(union uvarint token, size_t tokenlen, ngtcp2_cid original_dcid, ngtcp2_token_type *token_type)
{
	union uvarint token;
	size_t tokenlen = get_uvarint(pkt[len], token);
	if (tokenlen == 0)
	{
		return send_retry_packet(version, dcid, scid, original_dcid, addr);
	}
	else if (token.n8 != NGTCP2_CRYPTO_TOKEN_MAGIC_RETRY && dcid.datalen < NGTCP2_MIN_INITIAL_DCIDLEN)
	{
		return send_connection_close_packet(version, dcid, scid, 0, NULL, 0, addr, addrlen);
	}

	len += tokenlen + 1;
	if (pktlen < len)
	{
		return 0;
	}

	switch (token[0])
	{
		case NGTCP2_CRYPTO_TOKEN_MAGIC_RETRY:
		{
			res = ngtcp2_crypto_verify_retry_token(original_dcid, token, tokenlen, secret.data, secret.datalen, version,
			                                       sockaddr, sockaddr_len, dcid, RETRY_TIMEOUT, get_timestamp_ns());
			if (res == -1)
			{
				return send_connection_close_packet(version, dcid, scid, 0, NULL, 0, addr, addrlen);
			}

			*token_type = NGTCP2_TOKEN_TYPE_RETRY;
			return 0;
		}
		case NGTCP2_CRYPTO_TOKEN_MAGIC_REGULAR:
		{
			res = ngtcp2_crypto_verify_regular_token(token, tokenlen, secret.data, secret.datalen, sockaddr,
			                                         sockaddr_len, TIMEOUT, get_timestamp_ns());
			if (res == -1)
			{
				return send_retry_packet(version, dcid, scid, original_dcid, token, tokenlen, addr);
			}

			*token_type = NGTCP2_TOKEN_TYPE_NEW_TOKEN;
			return 0;
		}
		default:
		{
			return send_retry_packet(version, dcid, scid, original_dcid, token, tokenlen, addr);
		}
	}
}