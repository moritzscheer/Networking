// Copyright (C) 2024 Moritz Scheer

inline int verify_token(ngtcp2_cid *dcid, ngtcp2_cid *original_dcid, ngtcp2_pkt_hd header, ngtcp2_token_type token_type)
{
	int res;

	if (header.tokenlen == 0)
	{
		send_retry(&header, ep, local_addr, sockaddr, sockaddr_len, data.size() * 3);
		return;
	}

	if (header.token[0] != NGTCP2_CRYPTO_TOKEN_MAGIC_RETRY && header.dcid.datalen < NGTCP2_MIN_INITIAL_DCIDLEN)
	{
		send_stateless_connection_close(&header, ep, local_addr, sockaddr, sockaddr_len);
		return;
	}

	switch (header.token[0])
	{
		case NGTCP2_CRYPTO_TOKEN_MAGIC_RETRY:
			res = verify_retry_token(&original_dcid, &header, sockaddr, sockaddr_len)
			if (res != 0)
			{
				send_stateless_connection_close(&header, ep, local_addr, sockaddr, sockaddr_len);
				return res;
			}
			po_dcid = &original_dcid;
			token_type = NGTCP2_TOKEN_TYPE_RETRY;
			break;
		case NGTCP2_CRYPTO_TOKEN_MAGIC_REGULAR:
			res = verify_token(&header, sockaddr, sockaddr_len);
			if (res != 0)
			{
				send_retry(&header, ep, local_addr, sockaddr, sockaddr_len, data.size() * 3);
				return res;
			}
			else
			{
				token_type = NGTCP2_TOKEN_TYPE_NEW_TOKEN;
			}
			break;
		default:
			send_retry(&header, ep, local_addr, sockaddr, sockaddr_len, data.size() * 3);
			return;
	}
}

static inline int verify_retry_token(struct ngtcp2_cid *original_dcid, const struct ngtcp2_pkt_hd *header,
                                     const struct sockaddr *sockaddr, socklen_t sockaddr_len)
{
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];

	int res = getnameinfo(sockaddr, sockaddr_len, host, sizeof(host), port, sizeof(port),
	                      NI_NUMERICHOST | NI_NUMERICSERV);
	if (res != 0)
	{
		return res;
	}

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int64_t timestamp = ts.tv_sec * (int64_t) NGTCP2_SECONDS + ts.tv_nsec;

	return ngtcp2_crypto_verify_retry_token(
		original_dcid, header->token, header->tokenlen, secret->data, secret->datalen,
		header->version, sockaddr, sockaddr_len, &header->dcid, 10 * NGTCP2_SECONDS, timestamp);
}

static inline int verify_token(const struct ngtcp2_pkt_hd *header,
                               const struct sockaddr *sockaddr, socklen_t sockaddr_len)
{
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];

	int res = getnameinfo(sockaddr, sockaddr_len, host, sizeof(host), port, sizeof(port),
	                      NI_NUMERICHOST | NI_NUMERICSERV);
	if (res != 0)
	{
		return res;
	}

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int64_t timestamp = ts.tv_sec * (int64_t) NGTCP2_SECONDS + ts.tv_nsec;

	return ngtcp2_crypto_verify_regular_token(
		header->token, header->tokenlen, secret->data, secret->datalen,
		sockaddr, sockaddr_len, TIMEOUT, timestamp);
}