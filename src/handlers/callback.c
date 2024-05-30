

int init_ngtcp2()
{
	ngtcp2_callbacks callbacks = {

		nullptr,                                    /* client_initial */
		ngtcp2_crypto_recv_client_initial_cb,       /* recv_client_initial */
		recv_crypto_data,                           /* recv_crypto_data */
		handshake_completed,                        /* handshake_completed */
		nullptr,                                    /* recv_version_negotiation */
		ngtcp2_crypto_encrypt_cb                    /*  */
		ngtcp2_crypto_decrypt_cb,                   /*  */
		do_hp_mask,                                 /*  */
		recv_stream_data,                           /*  */
		acked_stream_data_offset,                   /*  */
		stream_open,
		stream_close,
		nullptr, // recv_stateless_reset
		nullptr, // recv_retry
		nullptr, // extend_max_local_streams_bidi
		nullptr, // extend_max_local_streams_uni
		rand,
		get_new_connection_id,
		remove_connection_id,
		update_key,
		path_validation,
		nullptr, // select_preferred_addr
		nullptr, // stream_reset
		nullptr, // extend_max_remote_streams_bidi
		nullptr, // extend_max_remote_streams_uni
		extend_max_stream_data,
		nullptr, // dcid_status
		nullptr, // handshake_confirmed
		nullptr, // recv_new_token
		ngtcp2_crypto_delete_crypto_aead_ctx_cb,
		ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
		nullptr, // recv_datagram
		nullptr, // ack_datagram
		nullptr, // lost_datagram
		ngtcp2_crypto_get_path_challenge_data_cb,
		nullptr, // stream_stop_sending
		ngtcp2_crypto_version_negotiation_cb,
		nullptr, // recv_rx_key
		nullptr // recv_tx_key
	}
}


int recv_crypto_data
	(ngtcp2_conn *conn,
	ngtcp2_encryption_level encryption_level,
	uint64_t offset,
	const uint8_t *data,
	size_t datalen,
	void *user_data)
{
	if (!config.quiet && !config.no_quic_dump)
	{
		print_crypto_data(encryption_level, {data, datalen});
	}

	return ngtcp2_crypto_recv_crypto_data_cb(conn, encryption_level, offset, data, datalen, user_data);
}

void print_crypto_data(ngtcp2_encryption_level encryption_level, uint8_t data)
{
	const char *encryption_level_str;

	switch (encryption_level) {
		case NGTCP2_ENCRYPTION_LEVEL_INITIAL:
			encryption_level_str = "Initial";
			break;
		case NGTCP2_ENCRYPTION_LEVEL_HANDSHAKE:
			encryption_level_str = "Handshake";
			break;
		case NGTCP2_ENCRYPTION_LEVEL_1RTT:
			encryption_level_str = "1-RTT";
			break;
		default:
			assert(0);
	}
	printf("Ordered CRYPTO data in %s crypto level\n", encryption_level_str);
}