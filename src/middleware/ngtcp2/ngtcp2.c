// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <ngtcp2/ngtcp2.h>
#include "ngtcp2.h"
#include "packet.h"
#include "../includes/status.h"
#include "../handlers/connection.h"
#include "../utils/utils.h"

int setup_ngtcp2(void)
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

	supported_versions = {NGTCP2_PROTO_VER_V1, NGTCP2_PROTO_VER_V2, NGTCP2_PROTO_VER_MAX};
	default_settings.preferred_versions = supported_versions;
	default_settings.preferred_versionslen = NUM_SUPPORTED_VERSIONS;

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
		return ENOMEM;
	}
	secret.datalen = SECRET_LEN;
	get_random(data, datalen);

	return 0;
}

int create_ngtcp2_conn(struct connection *connection, ngtcp2_cid *dcid, ngtcp2_cid *scid, uint32_t *version,
                       struct sockaddr_storage *addr, ngtcp2_token_type token_type, ngtcp2_cid *original_dcid)
{
	ngtcp2_settings settings;
	memcpy(&settings, &default_settings, sizeof(ngtcp2_settings));
	settings.initial_ts = get_timestamp_ns();
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
	}
	else
	{
		params.original_dcid = *scid;
	}
	params.original_dcid_present = 1;

	ngtcp2_path path = {
		.local = {
			.addrlen = server->local_addrlen,
			.addr = (struct sockaddr *) &local_addr
		},
		.remote = {
			.addrlen = sizeof(struct sockaddr_storage),
			.addr = (struct sockaddr *) &remote_addr
		}
	};

	if (ngtcp2_conn_server_new(&connection->, &header->scid, cid, &path, header.version, &callbacks,
	                           &settings, &params, NULL, connection) != 0)
	{
		return ENOMEM;
	}

	if (tls_session_.init(tls_ctx, this) != 0)
	{
		return -1;
	}

	tls_session_.enable_keylog();

	ngtcp2_conn_set_tls_native_handle(conn_, tls_session_.get_native_handle());

	return 0;
}

inline bool scid_is_valid(ngtcp2_cid cid, ngtcp2_conn *conn)
{
	if(!cid)
	{
		return true;
	}

	size_t num_scids = ngtcp2_conn_get_scid(conn, NULL);
	if (num_scids == 0)
	{
		return false;
	}

	ngtcp2_cid scids[num_scids];
	ngtcp2_conn_get_scid(conn, &scid);

	for(int i = 0; i < num_scids; i++)
	{
		if(ngtcp2_cid_eq(scid[i], cid))
		{
			return true;
		}
	}
	return false;
}
