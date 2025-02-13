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
#define RETRY_TIMEOUT 10 * NGTCP2_SECONDS

#define NUM_SUPPORTED_VERSIONS 3

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

static ngtcp2_settings default_settings;

static ngtcp2_transport_params default_params;

uint32_t supported_versions[NUM_SUPPORTED_VERSIONS];

/* ------------------------------------------- TYPEDEF DECLARATIONS ------------------------------------------------- */

typedef int *(*setup_ngtcp2)(void);

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int setup_ngtcp2(void);

int create_ngtcp2_conn(struct connection *connection, ngtcp2_cid *dcid, ngtcp2_cid *scid, uint32_t *version,
                       struct sockaddr_storage *addr, ngtcp2_token_type token_type, ngtcp2_cid *original_dcid);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_NGTCP2_H_