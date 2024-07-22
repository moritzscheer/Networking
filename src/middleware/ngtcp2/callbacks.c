// Copyright (C) 2024 Moritz Scheer

#include <ngtcp2/ngtcp2.h>
#include "callbacks.h"

extern ngtcp2_callbacks callbacks = {

	/* used callback function                   default callback functions */
	NULL,                                       // client_initial                           client only
	ngtcp2_crypto_recv_client_initial_cb,       // recv_client_initial
	ngtcp2_crypto_recv_crypto_data_cb,          // recv_crypto_data
	ngtcp2_handshake_completed,                 // handshake_completed
	NULL,                                       // recv_version_negotiation                 client only
	ngtcp2_crypto_encrypt_cb                    // encrypt
	ngtcp2_crypto_decrypt_cb,                   // decrypt
	ngtcp2_crypto_hp_mask_cb,                   // hp_mask
	recv_stream_data,                           // recv_stream_data
	acked_stream_data_offset,                   // acked_stream_data_offset
	stream_open,                                // stream_open
	stream_close,                               // stream_close
	ngtcp2_recv_stateless_reset,                // recv_stateless_reset
	NULL,                                       // recv_retry                               client only
	ngtcp2_extend_max_streams,                  // extend_max_local_streams_bidi
	ngtcp2_extend_max_streams,                  // extend_max_local_streams_uni
	ngtcp2_rand,                                // rand
	ngtcp2_get_new_connection_id,               // get_new_connection_id
	ngtcp2_remove_connection_id,                // remove_connection_id
	ngtcp2_crypto_update_key_cb,                // update_key
	path_validation,                            // path_validation
	NULL,                                       // select_preferred_addr
	stream_reset                                // stream_reset
	ngtcp2_extend_max_streams,                  // extend_max_remote_streams_bidi
	ngtcp2_extend_max_streams,                  // extend_max_remote_streams_uni
	ngtcp2_extend_max_stream_data,              // extend_max_stream_data
	NULL,                                       // dcid_status
	NULL,                                       // handshake_confirmed
	NULL,                                       // recv_new_token                           client only
	ngtcp2_crypto_delete_crypto_aead_ctx_cb,    // delete_crypto_aead_ctx
	ngtcp2_crypto_delete_crypto_cipher_ctx_cb,  // delete_crypto_cipher_ctx
	NULL,                                       // recv_datagram
	NULL,                                       // ack_datagram
	NULL,                                       // lost_datagram
	ngtcp2_crypto_get_path_challenge_data_cb,   // get_path_challenge_data
	NULL,                                       // stream_stop_sending
	ngtcp2_crypto_version_negotiation_cb,       // version_negotiation
	NULL,                                       // recv_rx_key
	NULL,                                       // recv_tx_key
	NULL                                        //    tls_early_data_rejected                  client only
}

int recv_stream_data_cb(ngtcp2_conn *conn, uint32_t flags, int64_t stream_id, uint64_t offset,
                        const uint8_t *data, size_t datalen, void *user_data, void *stream_user_data)
{
	struct connection *connection = (struct connection *) user_data;

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
	struct connection *connection = (struct connection *) user_data;

	Stream *stream = find_stream(connection, stream_id);
	if (stream)
	{
		stream_mark_acked(stream, offset + datalen);
	}

	return 0;
}

int stream_open_cb(ngtcp2_conn *conn, int64_t stream_id, void *user_data)
{
	struct connection *connection = (struct connection *) user_data;

	Stream *stream = create_stream(connection, stream_id);
	if (!stream)
	{
		return -1;
	}

	HASH_ADD_INT(connection->streams, stream_id, stream);
	return 0;
}

int stream_close_cb(ngtcp2_conn *conn, int64_t stream_id, void *user_data)
{
	struct connection *connection = (struct connection *) user_data;

	close_stream(connection);

	HASH_DEL(connection, stream);
	free(stream);
	return 0;
}

int stream_reset_cb(ngtcp2_conn *conn, int64_t stream_id, uint64_t final_size, uint64_t app_error_code,
                    void *user_data, void *stream_user_data)
{

}

ngtcp2_cid *find_scid(ngtcp2_conn *conn, ngtcp2_cid cid)
{
	size_t num_scids = ngtcp2_conn_get_scid(conn, NULL);
	if (num_scids == 0)
	{
		return NULL;
	}

	ngtcp2_cid *scid = malloc(sizeof(struct ngtcp2_cid) * num_scids);
	if(!scid)
	{
		return NULL;
	}

	ngtcp2_conn_get_scid(conn, scid);

	for(int i = 0; i < num_scids; i++)
	{
		if(ngtcp2_cid_eq(scid[i], cid))
		{
			free(scid);
			return cid;
		}
	}
	return NULL;
}