// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <ngtcp2/ngtcp2.h>
#include "ngtcp2.h"

int setup_ngtcp2()
{
	/*
	 * sets up default default_settings struct with custom initial value
	 */
	ngtcp2_settings_default(&default_settings);

	if (!qlog_dir.empty())
	{
		auto path = LOG_DIR;
		path += '/';
		path += '/';
		path += util::format_hex({scid.data, scid.datalen});
		path += ".sqlog";
		FILE *qlog_file = fopen(path.c_str(), "w");
		if (!qlog_file) {
			return -1;
		}
		default_settings.qlog_write = qlog();
	}

	default_settings.cc_algo = NGTCP2_CC_ALGO_CUBIC;
	default_settings.initial_rtt = NGTCP2_DEFAULT_INITIAL_RTT;
	default_settings.log_printf = log_function;
	default_settings.max_tx_udp_payload_size = MAX_TX_UDP_PAYLOAD_SIZE;
	default_settings.max_window = MAX_WINDOW;
	default_settings.max_stream_window = MAX_STREAM_WINDOW;
	default_settings.ack_thresh = ACK_THRESH;
	default_settings.no_tx_udp_payload_size_shaping = NO_TX_UDP_PAYLOAD_SIZE_SHAPING;
	default_settings.handshake_timeout = HANDSHAKE_TIMEOUT;
	default_settings.no_pmtud = NO_PMTUD;

	uint32_t versions[] = {NGTCP2_PROTO_VER_V1, NGTCP2_PROTO_VER_V2};
	default_settings.preferred_versions = versions;
	default_settings.preferred_versionslen = 2;

	uint16_t payload_size[] = [1300, 1400, NGTCP2_MAX_UDP_PAYLOAD_SIZE];
	default_settings.pmtud_probes = payload_size;
	default_settings.pmtud_probeslen = 3;

	/*
	 * sets up default ngtcp2_transport_params struct with custom initial value
	 */
	ngtcp2_transport_params_default(&default_params);

	default_params.initial_max_stream_data_bidi_local = INITIAL_MAX_STREAM_DATA_BIDI_LOCAL;
	default_params.initial_max_stream_data_bidi_remote = INITIAL_MAX_STREAM_DATA_BIDI_REMOTE;
	default_params.initial_max_stream_data_uni = INITIAL_MAX_STREAM_DATA_UNI;
	default_params.initial_max_data = INITIAL_MAX_DATA;
	default_params.initial_max_streams_bidi = INITIAL_MAX_STREAMS_BIDI;
	default_params.initial_max_streams_uni = INITIAL_MAX_STREAMS_UNI;
	default_params.max_idle_timeout = MAX_IDLE_TIMEOUT;
	default_params.active_connection_id_limit = ACTIVE_CONNECTION_ID_LIMIT;
	default_params.stateless_reset_token_present = 1;
	default_params.grease_quic_bit = 1;

	/*
	 * generates a secret
	 */
	secret.data = calloc(1, sizeof(uint8_t) * SECRET_LEN);
	if(!secret.data)
	{
		return NOMEM;
	}
	secret.datalen = SECRET_LEN;
	get_random(data, datalen);

	return 0;
}

int create_ngtcp2_conn(ngtcp2_cid cid, uint8_t *pkt, size_t pktlen,
					  struct sockaddr_union remote_addr, size_t remote_addrlen)
{
	int res;

	ngtcp2_pkt_hd header;
	res = ngtcp2_accept(&header, pkt, pktlen);
	if(res != 0)
	{
		return res;
	}

	ngtcp2_cid original_dcid;
	ngtcp2_token_type token_type;
	res = verify_token(cid, original_dcid, header, &token_type);
	if(res != 0)
	{
		return res;
	}

	ngtcp2_settings settings;
	memcpy(&settings, &default_settings, sizeof(ngtcp2_settings));
	settings.initial_ts = timestamp_ns();
	settings.token = header->token;
	settings.tokenlen = header->tokenlen;
	settings.token_type = token_type;
	settings.initial_pkt_num = get_random(sizeof(uint32_t));

	ngtcp2_transport_params params;
	memcpy(&params, &default_params, sizeof(ngtcp2_transport_params));
	if (ocid)
	{
		params.original_dcid = *ocid;
		params.retry_scid = *scid;
		params.retry_scid_present = 1;
	} else
	{
		params.original_dcid = *scid;
	}
	params.original_dcid_present = 1;

	ngtcp2_path path =
		{
			.local = {
				.addrlen = server->local_addrlen,
				.addr = (struct sockaddr *) &server->local_addr
			},
			.remote = {

			}
		};

	ngtcp2_conn *conn = NULL;
	if (ngtcp2_conn_server_new(&conn, &header->scid, cid, &path, header.version, &callbacks,
	                           &settings, &params, NULL, connection) != 0)
	{
		return NOMEM;
	}

	if (tls_session_.init(tls_ctx, this) != 0)
	{
		return -1;
	}

	tls_session_.enable_keylog();

	ngtcp2_conn_set_tls_native_handle(conn_, tls_session_.get_native_handle());

	return 0;
}

int rest()
{
	if (ngtcp2_crypto_generate_stateless_reset_token(
		params.stateless_reset_token, static_secret.data(),
		config->static_secret.size(), &scid) != 0) {
		return -1;
	}

	if (preferred_ipv4_addr.len || preferred_ipv6_addr.len) {
		params.preferred_addr_present = 1;

		if (preferred_ipv4_addr.len) {
			params.preferred_addr.ipv4 = preferred_ipv4_addr.su.in;
			params.preferred_addr.ipv4_present = 1;
		}

		if (preferred_ipv6_addr.len) {
			params.preferred_addr.ipv6 = preferred_ipv6_addr.su.in6;
			params.preferred_addr.ipv6_present = 1;
		}

		if (util::generate_secure_random() != 0) {
			std::cerr << "Could not generate preferred address stateless reset token"
			          << std::endl;
			return -1;
		}

		res = get_random(params.preferred_addr.stateless_reset_token, sizeof(uint8_t));
		if(res != 0)
		{
			return res;
		}

		params.preferred_addr.cid.datalen = NGTCP2_SV_SCIDLEN;
		res = get_random(&params.preferred_addr.cid.data, params.preferred_addr.cid.datalen);
		if(res != 0)
		{
			return res;
		}

		if (util::generate_secure_random({params.preferred_addr.cid.data, params.preferred_addr.cid.datalen}) != 0) {
			std::cerr << "Could not generate preferred address connection ID"
			          << std::endl;
			return -1;
		}
	}
}

static inline int verify_token(ngtcp2_cid *dcid, ngtcp2_cid *original_dcid, ngtcp2_pkt_hd header, ngtcp2_token_type token_type)
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