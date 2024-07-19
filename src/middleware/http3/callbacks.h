// Copyright (C); 2024 Moritz Scheer

#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

nghttp3_callbacks callbacks;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

typedef int (*acked_stream_data_cb)(nghttp3_conn *conn, int64_t stream_id, uint64_t datalen, void *conn_user_data,
                                    void *stream_user_data);

typedef int (*recv_data_cb)(nghttp3_conn *conn, int64_t stream_id, const uint8_t *data, size_t datalen,
                            void *conn_user_data, void *stream_user_data);

typedef int (*deferred_consume_cb)(nghttp3_conn *conn, int64_t stream_id, size_t consumed, void *conn_user_data,
                                   void *stream_user_data);

typedef int (*begin_headers_cb)(nghttp3_conn *conn, int64_t stream_id, void *conn_user_data, void *stream_user_data);

typedef int (*recv_header_cb)(nghttp3_conn *conn, int64_t stream_id, int32_t token, nghttp3_rcbuf *name,
                              nghttp3_rcbuf *value, uint8_t flags, void *conn_user_data, void *stream_user_data);

typedef int (*end_headers_cb)(nghttp3_conn *conn, int64_t stream_id, int fin, void *conn_user_data,
                              void *stream_user_data);

typedef int (*end_stream_cb)(nghttp3_conn *conn, int64_t stream_id, void *conn_user_data, void *stream_user_data);

typedef int (*stream_close_cb)(nghttp3_conn *conn, int64_t stream_id, uint64_t app_error_code, void *conn_user_data,
                               void *stream_user_data);

typedef int (*reset_stream_cb)(nghttp3_conn *conn, int64_t stream_id, uint64_t app_error_code, void *conn_user_data,
                               void *stream_user_data);

typedef int (*stop_sending_cb)(nghttp3_conn *conn, int64_t stream_id, uint64_t app_error_code, void *conn_user_data,
                               void *stream_user_data);

typedef int (*shutdown_cb)(nghttp3_conn *conn, int64_t id, void *conn_user_data);

typedef int (*recv_settings_cb)(nghttp3_conn *conn, const nghttp3_settings *settings, void *conn_user_data);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_CALLBACKS_H_
