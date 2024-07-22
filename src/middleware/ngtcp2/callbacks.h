// Copyright (C) 2024 Moritz Scheer

#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

ngtcp2_callbacks callbacks;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

typedef int (*recv_stream_data_cb)(ngtcp2_conn *conn, uint32_t flags, int64_t stream_id, uint64_t offset,
                                   const uint8_t *data, size_t datalen, void *user_data, void *stream_user_data);

typedef int (*acked_stream_data_offset_cb)(ngtcp2_conn *conn, int64_t stream_id, uint64_t offset, uint64_t datalen,
                                           void *user_data, void *stream_user_data);

typedef int (*stream_open_cb)(ngtcp2_conn *conn, int64_t stream_id, void *user_data);

typedef int (*stream_close_cb)(ngtcp2_conn *conn, int64_t stream_id, void *user_data);

typedef int (*extend_max_local_streams_bidi_cb)(ngtcp2_conn *conn, uint64_t max_streams, void *user_data);

typedef int (*extend_max_local_streams_uni_cb)(ngtcp2_conn *conn, uint64_t max_streams, void *user_data);

typedef int (*extend_max_remote_streams_bidi_cb)(ngtcp2_conn *conn, uint64_t max_streams, void *user_data);

typedef int (*extend_max_remote_streams_uni_cb)(ngtcp2_conn *conn, uint64_t max_streams, void *user_data);

typedef int (*stream_reset_cb)(ngtcp2_conn *conn, int64_t stream_id, uint64_t final_size, uint64_t app_error_code,
                               void *user_data, void *stream_user_data);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_CALLBACKS_H_
