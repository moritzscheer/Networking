// Copyright (C) 2024 Moritz Scheer

#include "connection.h"

struct stream *create_stream(struct connection *connection, int64_t stream_id)
{
	struct stream *stream = find_stream(connection, stream_id);
	if (stream)
	{
		return -1;
	}

	if (check_streams_left(connection->ngtcp2_conn, stream_id) <= 0)
	{
		return -1;
	}

	stream = (struct stream *) calloc(1, sizeof(struct stream));
	if (!stream)
	{
		return -1;
	}

	stream->data = queue_new();
	if (!stream->data)
	{
		free(stream);
		return -1;
	}

	stream->id = stream_id;
	stream->acked_offset = 0;
	stream->sent_offset = 0;

	return stream;
}

struct stream *find_stream(struct connection *connection, int64_t stream_id)
{
	struct stream *stream;
	HASH_FIND_INT(connection->streams, &stream_id, stream);
	return stream;
}

inline int conn_check_streams_left(ngtcp2_conn conn, int64_t stream_id)
{
	int streams_left;

	if (ngtcp2_is_bidi_stream(stream_id))
	{
		return ngtcp2_conn_get_streams_bidi_left(conn);
	}
	return ngtcp2_conn_get_streams_uni_left(conn);
}
