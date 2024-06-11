// Copyright (C) 2024 Moritz Scheer

int stream_open_cb(ngtcp2_conn *conn, int64_t stream_id, void *user_data)
{
	switch (stream_id % 4)
	{
		case 0: /* Bidirectional Stream */
			if (ngtcp2_conn_get_streams_bidi_left(conn) <= 0)
			{
				return NGTCP2_ERR_STREAM_LIMIT;
			}
			break;
		case 2: /* Unidirectional Stream */
			if (ngtcp2_conn_get_streams_uni_left(conn) <= 0)
			{
				return NGTCP2_ERR_STREAM_LIMIT;
			}
			break;
		default:
			return NGTCP2_ERR_CALLBACK_FAILURE;
	}

	Stream *stream = (Stream *) calloc(1, sizeof(Stream));
	if (!stream)
	{
		return NULL;
	}

	stream->data = queue_new();
	if (!stream->data)
	{
		free(stream);
		return NULL;
	}

	stream->id = id;
	stream->acked_offset = 0;
	stream->sent_offset = 0;

	HASH_ADD_INT((Connection *) user_data, stream->id, stream);
	return 0;
}

int stream_close_cb(ngtcp2_conn *conn, int64_t stream_id, void *user_data)
{
	Connection *connection = (Connection *) user_data;

	Stream *stream = find_stream(connection, stream_id);
	if (!stream)
	{
		return 0;
	}

	// Do any necessary processing before closing the stream
	// For example, you might want to process any remaining data in the stream buffer
	close_stream(stream);

	HASH_DEL(connection, stream);
	free(stream);
	return 0;
}

int recv_stream_data_cb(ngtcp2_conn *conn, uint32_t flags, int64_t stream_id, uint64_t offset,
                        const uint8_t *data, size_t datalen, void *user_data, void *stream_user_data)
{
	Connection *connection = (Connection *) user_data;

	Stream *stream = find_stream(connection, stream_id);
	if (stream)
	{
		stream_push_data(stream, data, datalen);
	}
	return 0;
}

int acked_stream_data_offset_cb(ngtcp2_conn *conn, int64_t stream_id, uint64_t offset, uint64_t datalen,
                                void *user_data, void *stream_user_data)
{
	Connection *connection = (Connection *) user_data;

	Stream *stream = find_stream(connection, stream_id);
	if (stream)
	{
		stream_mark_acked(stream, offset + datalen);
	}
	return 0;
}

int stream_push_data(Stream *stream, const uint8_t *data, size_t data_size)
{
	g_return_val_if_fail(bytes, -1);
	g_queue_push_tail(stream->buffer, bytes);
	return 0;
}

void stream_finish_data()
{

}

Stream *find_stream(Connection *connection, int64_t id)
{

}

