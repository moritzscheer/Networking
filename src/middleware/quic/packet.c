// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "packet.h"
#include "../../middleware/quic/ngtcp2.h"
#include "../../includes/ring.h"
#include "../../includes/server.h"

int create_packet(ngtcp2_conn *conn, uint8_t *data, size_t datalen)
{
	mark_packet_ecn(data, datalen, ECT_0);
}

inline int send_connection_close_packet(uint32_t version, const ngtcp2_cid *dcid, const ngtcp2_cid *scid,
                                        uint64_t error_code, const uint8_t *reason, size_t reasonlen)
{
	uint8_t *buf = calloc(1, BUF_SIZE);
	if (!buf)
	{
		return NOMEM;
	}

	size_t len = ngtcp2_crypto_write_connection_close(buf, BUF_SIZE, version, dcid, scid, error_code,
	                                                  reason, reasonlen);

	uint8_t *resized_buf = realloc(buf, len);
	free(buf);
	if (!resized_buf)
	{
		return NOMEM;
	}
	return prepare_write(resized_buf, len);
}

inline int send_retry_packet(uint32_t version, const ngtcp2_cid *dcid, const ngtcp2_cid *scid, const ngtcp2_cid *odcid,
                             const uint8_t *token, size_t tokenlen)
{
	uint8_t *buf = calloc(1, BUF_SIZE);
	if (!buf)
	{
		return NOMEM;
	}

	size_t len = ngtcp2_crypto_write_retry(buf, BUF_SIZE, version, dcid, scid, odcid, token, tokenlen);

	uint8_t *resized_buf = realloc(buf, len);
	free(buf);
	if (!resized_buf)
	{
		return NOMEM;
	}
	return prepare_write(resized_buf, len);
}

inline int send_stateless_reset_packet(const uint8_t *stateless_reset_token)
{
	uint8_t *buf = calloc(1, BUF_SIZE);
	if (!buf)
	{
		return NOMEM;
	}

	uint8_t random[NGTCP2_MIN_STATELESS_RESET_RANDLEN];
	res = get_random(&random, NGTCP2_MIN_STATELESS_RESET_RANDLEN);
	if (res != 0)
	{
		return res;
	}

	size_t len = ngtcp2_pkt_write_stateless_reset(buf, BUF_SIZE, stateless_reset_token, random,
	                                              NGTCP2_MIN_STATELESS_RESET_RANDLEN);

	uint8_t *resized_buf = realloc(buf, len);
	free(buf);
	if (!resized_buf)
	{
		return NOMEM;
	}
	return prepare_write(resized_buf, len);
}

inline int send_version_negotiation_packet(const uint8_t *dcid, size_t dcidlen, const uint8_t *scid, size_t scidlen)
{
	uint8_t *buf = calloc(1, BUF_SIZE);
	if (!buf)
	{
		return NOMEM;
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
		return NOMEM;
	}
	return prepare_write(resized_buf, len);
}