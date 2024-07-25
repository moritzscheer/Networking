// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "packet.h"
#include "../../handlers/write/write.h"
#include "../../middleware/quic/ngtcp2.h"
#include "../../includes/ring.h"
#include "../../includes/server.h"
#include "../../includes/status.h"
#include "../../utils/utils.h"

int create_packet(ngtcp2_conn *conn, uint8_t *data, size_t datalen)
{
	mark_packet_ecn(data, datalen, ECT_0);
}

int verify_initial_packet(ngtcp2_cid *dcid, uint32_t *version, ngtcp2_token_type token_type, ngtcp2_cid *original_dcid,
                          ngtcp2_path path)
{
	ngtcp2_pkt_hd header;
	res = ngtcp2_accept(&header, pkt, pktlen);
	if(res != 0)
	{
		return NGTCP2_ERR_STATELESS_RESET;
	}

	if (header.tokenlen == 0)
	{
		return NGTCP2_ERR_RETRY;
	}

	if (header.token[0] != NGTCP2_CRYPTO_TOKEN_MAGIC_RETRY && header.dcid.datalen < NGTCP2_MIN_INITIAL_DCIDLEN)
	{
		return NGTCP2_ERR_CONNECTION_CLOSE;
	}

	switch (header.token[0])
	{
		case NGTCP2_CRYPTO_TOKEN_MAGIC_RETRY:
		{
			res = verify_address(addr);
			if(res != 0)
			{
				return res;
			}

			ngtcp2_tstamp timestamp = get_timestamp_ns();

			res = ngtcp2_crypto_verify_retry_token(original_dcid, header.token, header.tokenlen, secret.data,
			                                       secret.datalen, header.version, sockaddr, sockaddr_len,
			                                       header.dcid, RETRY_TIMEOUT, timestamp);
			if (res == 0)
			{
				return NGTCP2_ERR_CONNECTION_CLOSE;
			}

			token_type = NGTCP2_TOKEN_TYPE_RETRY;
			return 0;
		}
		case NGTCP2_CRYPTO_TOKEN_MAGIC_REGULAR:
		{
			res = verify_address(addr);
			if(res != 0)
			{
				return res;
			}

			ngtcp2_tstamp timestamp = get_timestamp_ns();

			res = ngtcp2_crypto_verify_regular_token(header.token, header.tokenlen, secret.data, secret.datalen,
			                                         sockaddr, sockaddr_len, TIMEOUT, timestamp);
			if (res == 0)
			{
				return NGTCP2_ERR_RETRY;
			}

			token_type = NGTCP2_TOKEN_TYPE_NEW_TOKEN;
			return 0;
		}
		default:
		{
			return NGTCP2_ERR_RETRY;
		}
	}
}

inline int send_connection_close_packet(uint32_t version, const ngtcp2_cid *dcid, const ngtcp2_cid *scid,
                                        uint64_t error_code, const uint8_t *reason, size_t reasonlen,
                                        struct sockaddr_storage *addr)
{
	uint8_t *buf = calloc(1, BUF_SIZE);
	if (!buf)
	{
		return ENOMEM;
	}

	size_t len = ngtcp2_crypto_write_connection_close(buf, BUF_SIZE, version, dcid, scid, error_code,
	                                                  reason, reasonlen);

	uint8_t *resized_buf = realloc(buf, len);
	free(buf);
	if (!resized_buf)
	{
		return ENOMEM;
	}
	return prepare_write(resized_buf, len);
}

inline int send_retry_packet(uint32_t version, const ngtcp2_cid *dcid, const ngtcp2_cid *scid,
                             const ngtcp2_cid *original_dcid, const uint8_t *token, size_t tokenlen
                             struct sockaddr_storage *addr)
{
	uint8_t *buf = calloc(1, BUF_SIZE);
	if (!buf)
	{
		return ENOMEM;
	}

	size_t len = ngtcp2_crypto_write_retry(buf, BUF_SIZE, version, dcid, scid, odcid, token, tokenlen);

	uint8_t *resized_buf = realloc(buf, len);
	free(buf);
	if (!resized_buf)
	{
		return ENOMEM;
	}
	return prepare_write(resized_buf, len);
}

inline int send_stateless_reset_packet(ngtcp2_cid *dcid, struct sockaddr_storage *addr)
{
	uint8_t *buf = calloc(1, BUF_SIZE);
	if (!buf)
	{
		return ENOMEM;
	}

	uint8_t random[NGTCP2_MIN_STATELESS_RESET_RANDLEN];
	res = get_random(&random, NGTCP2_MIN_STATELESS_RESET_RANDLEN);
	if (res != 0)
	{
		return res;
	}

	uint8_t token;
	res = ngtcp2_crypto_generate_stateless_reset_token(&token, secret.data, secret.datalen, dcid)
	if(res != 0)
	{
		return res;
	}

	size_t len = ngtcp2_pkt_write_stateless_reset(buf, BUF_SIZE, &token, &random, NGTCP2_MIN_STATELESS_RESET_RANDLEN);

	uint8_t *resized_buf = realloc(buf, len);
	free(buf);
	if (!resized_buf)
	{
		return ENOMEM;
	}
	return prepare_write(resized_buf, len);
}

inline int send_version_negotiation_packet(const uint8_t *dcid, size_t dcidlen, const uint8_t *scid, size_t scidlen,
                                           struct sockaddr_storage *addr)
{
	uint8_t *buf = calloc(1, BUF_SIZE);
	if (!buf)
	{
		return ENOMEM;
	}

	uint8_t random;
	res = get_random(&random, sizeof(uint8_t));
	if (res != 0)
	{
		return res;
	}

	size_t len = ngtcp2_pkt_write_version_negotiation(buf, BUF_SIZE, random, dcid, dcidlen, scid, sclidlen,
	                                                  supported_versions, NUM_SUPPORTED_VERSIONS);

	uint8_t *resized_buf = realloc(buf, len);
	free(buf);
	if (!resized_buf)
	{
		return ENOMEM;
	}
	return prepare_write(resized_buf, len);
}