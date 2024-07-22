// Copyright (C) 2024 Moritz Scheer

extern nghttp3_callbacks callbacks = {

	/* used callback function                   default callback functions */
	acked_stream_data_cb,                       // acked_stream_data                           client only
	stream_close_cb,                            // stream_close
	recv_data_cb,                               // recv_data
	deferred_consume_cb,                        // deferred_consume
	begin_headers_cb,                           // begin_headers
	recv_header_cb,                             // recv_header
	end_headers_cb,                             // end_headers
	begin_headers_cb,                           // begin_trailers
	recv_header_cb,                             // recv_trailer
	end_headers_cb,                             // end_trailers
	stop_sending_cb,                            // stop_sending
	end_stream_cb,                              // end_stream
	reset_stream_cb,                            // reset_stream
	shutdown_cb,                                // shutdown
	recv_settings_cb,                           // recv_settings
}

int acked_stream_data_cb(nghttp3_conn *conn, int64_t stream_id, uint64_t datalen, void *conn_user_data, void *stream_user_data)
{

}

int recv_data_cb(nghttp3_conn *conn, int64_t stream_id, const uint8_t *data, size_t datalen, void *conn_user_data, void *stream_user_data)
{

}

int deferred_consume_cb(nghttp3_conn *conn, int64_t stream_id, size_t consumed, void *conn_user_data, void *stream_user_data)
{

}

int begin_headers_cb(nghttp3_conn *conn, int64_t stream_id, void *conn_user_data, void *stream_user_data)
{

}

int recv_header_cb(nghttp3_conn *conn, int64_t stream_id, int32_t token, nghttp3_rcbuf *name, nghttp3_rcbuf *value, uint8_t flags, void *conn_user_data, void *stream_user_data)
{

}

int end_headers_cb(nghttp3_conn *conn, int64_t stream_id, int fin, void *conn_user_data, void *stream_user_data)
{

}

int end_stream_cb(nghttp3_conn *conn, int64_t stream_id, void *conn_user_data, void *stream_user_data)
{

}

int stream_close_cb(nghttp3_conn *conn, int64_t stream_id, uint64_t app_error_code, void *conn_user_data, void *stream_user_data)
{

}

int reset_stream_cb(nghttp3_conn *conn, int64_t stream_id, uint64_t app_error_code, void *conn_user_data, void *stream_user_data)
{

}

int stop_sending_cb(nghttp3_conn *conn, int64_t stream_id, uint64_t app_error_code, void *conn_user_data, void *stream_user_data)
{

}

int shutdown_cb(nghttp3_conn *conn, int64_t id, void *conn_user_data)
{

}

int recv_settings_cb(nghttp3_conn *conn, const nghttp3_settings *settings, void *conn_user_data)
{

}