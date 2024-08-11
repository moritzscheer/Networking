// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "packet.h"
#include "../../handlers/connection.h"
#include "../../handlers/write/write.h"
#include "../../middleware/ngtcp2/ngtcp2.h"
#include "../../includes/ring.h"
#include "../../includes/server.h"
#include "../../includes/status.h"
#include "../../utils/utils.h"

int verify_initial_packet(ngtcp2_token_type token_type, ngtcp2_cid *original_dcid, struct rqe *entry)
{
	ngtcp2_pkt_hd header;
	res = ngtcp2_accept(&header, pkt, pktlen);
	if(res != 0)
	{
		return send_stateless_reset_packet(entry->dcid, entry->addr);
	}

	if (header.tokenlen == 0)
	{
		return send_retry_packet(header.version, header.dcid, header.scid, original_dcid, header.token,
								 header.tokenlen, entry->addr);
	}

	if (header.token[0] != NGTCP2_CRYPTO_TOKEN_MAGIC_RETRY && header.dcid.datalen < NGTCP2_MIN_INITIAL_DCIDLEN)
	{
		return send_connection_close_packet(header.version, header.dcid, header.scid, 0, NULL, 0, addr, addrlen);
	}

	switch (header.token[0])
	{
		case NGTCP2_CRYPTO_TOKEN_MAGIC_RETRY:
		{
			ngtcp2_tstamp timestamp = get_timestamp_ns();

			res = ngtcp2_crypto_verify_retry_token(original_dcid, header.token, header.tokenlen, secret.data,
			                                       secret.datalen, header.version, sockaddr, sockaddr_len,
			                                       header.dcid, RETRY_TIMEOUT, timestamp);
			if (res == 0)
			{
				return send_connection_close_packet(header.version, header.dcid, header.scid, 0, NULL, 0, addr, addrlen);
			}

			token_type = NGTCP2_TOKEN_TYPE_RETRY;
			return 0;
		}
		case NGTCP2_CRYPTO_TOKEN_MAGIC_REGULAR:
		{
			ngtcp2_tstamp timestamp = get_timestamp_ns();

			res = ngtcp2_crypto_verify_regular_token(header.token, header.tokenlen, secret.data, secret.datalen,
			                                         sockaddr, sockaddr_len, TIMEOUT, timestamp);
			if (res == 0)
			{
				return send_retry_packet(header.version, header.dcid, header.scid, original_dcid, header.token,
				                         header.tokenlen, entry->addr);
			}

			token_type = NGTCP2_TOKEN_TYPE_NEW_TOKEN;
			return 0;
		}
		default:
		{
			return send_retry_packet(header.version, header.dcid, header.scid, original_dcid, header.token,
			                         header.tokenlen, entry->addr);
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

	ssize_t len = ngtcp2_crypto_write_connection_close(buf, BUF_SIZE, version, dcid, scid, error_code,
	                                                  reason, reasonlen);
	uint8_t *resized_buf = realloc(buf, len);
	if(!resized_buf)
	{
		free(buf);
		return ENOMEM;
	}
	return prepare_write(resized_buf, len);
}

inline int send_retry_packet(uint32_t version, const ngtcp2_cid *dcid, const ngtcp2_cid *scid,
                             const ngtcp2_cid *original_dcid, struct sockaddr_storage *addr, socklen_t addrlen,
								 ngtcp2_tstamp timestamp)
{
	uint8_t *buf = calloc(1, BUF_SIZE);
	if (!buf)
	{
		return ENOMEM;
	}

	ngtcp2_cid retry_scid;
	retry_scid.datalen = NGTCP_MAX_CIDLEN;
	res = get_random(&cid.data, NGTCP2_MAX_CIDLEN);
	if(res != 0)
	{
		return res;
	}

	uint8_t token;
	ssize_t tokenlen = ngtcp2_crypto_generate_retry_token(&token, secret.data, secret.datalen, version, addr, addrlen,
														  retry_scid, original_dcid, timestamp);
	if(tokenlen == -1)
	{
		return 0;
	}

	ssize_t len = ngtcp2_crypto_write_retry(buf, BUF_SIZE, version, dcid, scid, original_dcid, &token, tokenlen);
	uint8_t *resized_buf = realloc(buf, len);
	if(!resized_buf)
	{
		free(buf);
		return ENOMEM;
	}
	return prepare_write(resized_buf, len);
}

inline int send_stateless_reset_packet(ngtcp2_cid *dcid, struct sockaddr_storage *addr, socklen_t addrlen)
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

	ssize_t len = ngtcp2_pkt_write_stateless_reset(buf, BUF_SIZE, &token, &random, NGTCP2_MIN_STATELESS_RESET_RANDLEN);
	uint8_t *resized_buf = realloc(buf, len);
	if(!resized_buf)
	{
		free(buf);
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

	ssize_t len = ngtcp2_pkt_write_version_negotiation(buf, BUF_SIZE, random, dcid, dcidlen, scid, sclidlen,
	                                                  supported_versions, NUM_SUPPORTED_VERSIONS);
	uint8_t *resized_buf = realloc(buf, len);
	if(!resized_buf)
	{
		free(buf);
		return ENOMEM;
	}
	return prepare_write(resized_buf, len);
}

char *make_status_body(unsigned int status_code)
{
	auto status_string = util::format_uint(status_code);
	auto reason_phrase = http::get_reason_phrase(status_code);

	std::string body;
	body = "<html><head><title>";
	body += status_string;
	body += ' ';
	body += reason_phrase;
	body += "</title></head><body><h1>";
	body += status_string;
	body += ' ';
	body += reason_phrase;
	body += "</h1><hr><address>";
	body += NGTCP2_SERVER;
	body += " at port ";
	body += util::format_uint(config.port);
	body += "</address>";
	body += "</body></html>";
	return body;
}