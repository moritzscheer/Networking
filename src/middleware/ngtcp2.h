// Copyright (C) 2024 Moritz Scheer

#ifndef _NGTCP2_H_
#define _NGTCP2_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

/* settings macros */
#define MAX_WINDOW 1
#define MAX_STREAM_WINDOW 0
#define HANDSHAKE_TIMEOUT UINT64_MAX 0
#define NO_PMTUD 0
#define ACK_THRESH 2
#define MAX_TX_UDP_PAYLOAD_SIZE MAX_TX_UDP_PAYLOAD_SIZE
#define NO_TX_UDP_PAYLOAD_SIZE_SHAPING 1
#define MAX_STREAM_DATA_BIDI_LOCAL 256 * 1024 // 265 KB

/* transport params macros */
#define INITIAL_MAX_STREAM_DATA_BIDI_LOCAL 128 * 1024
#define INITIAL_MAX_STREAM_DATA_BIDI_REMOTE = 128 * 1024
#define INITIAL_MAX_STREAM_DATA_UNI
#define INITIAL_MAX_DATA = 1024 * 1024
#define INITIAL_MAX_STREAMS_BIDI 100
#define INITIAL_MAX_STREAMS_UNI 50
#define MAX_IDLE_TIMEOUT 0
#define ACTIVE_CONNECTION_ID_LIMIT 7

/* secret macros */
#define SECRET_LEN 32
#define TIMEOUT 3600 * NGTCP2_SECONDS

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

struct secret
{
	size_t datalen;
	uint8_t *data;
};

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

static struct secret server_secret;

static ngtcp2_settings default_settings;

static ngtcp2_transport_params default_params;

static ngtcp2_callbacks callbacks = {

	/* used callback function                   default callback functions */
	nullptr,                                    /* client_initial */
	ngtcp2_crypto_recv_client_initial_cb,       /* recv_client_initial */
	recv_crypto_data,                           /* recv_crypto_data */
	handshake_completed,                        /* handshake_completed */
	nullptr,                                    /* recv_version_negotiation */
	ngtcp2_crypto_encrypt_cb                    /* encrypt */
	ngtcp2_crypto_decrypt_cb,                   /* decrypt */
	do_hp_mask,                                 /* hp_mask */
	recv_stream_data_cb,                        /* recv_stream_data */
	acked_stream_data_offset_cb,                /* acked_stream_data_offset */
	stream_open_cb,                             /* stream_open */
	stream_close_cb,                            /* stream_close */
	nullptr,                                    /* recv_stateless_reset */
	nullptr,                                    /* recv_retry */
	nullptr,                                    /* extend_max_local_streams_bidi */
	nullptr,                                    /* extend_max_local_streams_uni */
	rand,                                       /* rand */
	get_new_connection_id,                      /* get_new_connection_id */
	remove_connection_id,                       /* remove_connection_id */
	update_key,                                 /* update_key */
	path_validation,                            /* path_validation */
	nullptr,                                    /* select_preferred_addr */
	nullptr,                                    /* stream_reset */
	nullptr,                                    /* extend_max_remote_streams_bidi */
	nullptr,                                    /* extend_max_remote_streams_uni */
	extend_max_stream_data,                     /* extend_max_stream_data */
	nullptr,                                    /* dcid_status */
	nullptr,                                    /* handshake_confirmed */
	nullptr,                                    /* recv_new_token */
	ngtcp2_crypto_delete_crypto_aead_ctx_cb,    /* delete_crypto_aead_ctx */
	ngtcp2_crypto_delete_crypto_cipher_ctx_cb,  /* delete_crypto_cipher_ctx */
	nullptr,                                    /* recv_datagram */
	nullptr,                                    /* ack_datagram */
	nullptr,                                    /* lost_datagram */
	ngtcp2_crypto_get_path_challenge_data_cb,   /* get_path_challenge_data */
	nullptr,                                    /* stream_stop_sending */
	ngtcp2_crypto_version_negotiation_cb,       /* version_negotiation */
	nullptr,                                    /* recv_rx_key */
	nullptr,                                    /* recv_tx_key */
	nullptr                                     /* tls_early_data_rejected */
}

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int setup_ngtcp2();

int create_ngtcp2_conn(ngtcp2_cid cid, uint8_t *pkt, size_t pktlen,
                       struct sockaddr_union remote_addr, size_t remote_addrlen);

static int verify_token(ngtcp2_cid *dcid, ngtcp2_cid *original_dcid, ngtcp2_pkt_hd header,
                        ngtcp2_token_type token_type);

static inline int verify_retry_token(struct ngtcp2_cid *original_dcid, const struct ngtcp2_pkt_hd *header,
                                     const struct sockaddr *sockaddr, socklen_t sockaddr_len);

static inline int verify_token(const struct ngtcp2_pkt_hd *header,
                               const struct sockaddr *sockaddr, socklen_t sockaddr_len);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_NGTCP2_H_