// Copyright (C) 2024 Moritz Scheer

Stream *create_stream(int64_t id)
{
	Stream *stream = (Stream *) malloc(sizeof(Stream));
	if (!stream)
	{
		return NULL;
	}
	stream->id = id;
	stream->buffer = queue_new();
	if (!stream->buffer)
	{
		free(stream);
		return NULL;
	}
	stream->acked_offset = 0;
	stream->sent_offset = 0;
	return stream;
}

void close_stream(Stream *stream)
{
	if (!stream)
	{
		return;
	}

	g_queue_free_full(stream->buffer, (GDestroyNotify) g_bytes_unref);
	g_free(stream);
}

int stream_push_data()
{

}

void stream_finish_data()
{

}

static int recv_stream_data_cb(ngtcp2_conn *conn,
                               uint32_t flags,
                               int64_t stream_id,
                               uint64_t offset,
                               const uint8_t *data, size_t datalen,
                               void *user_data,
                               void *stream_user_data)
{
	Connection *connection = user_data;
	Stream *stream = connection_find_stream(connection, stream_id);

	if (stream)
	{
		stream_push_data(stream, data, datalen);
	}

	return 0;
}
