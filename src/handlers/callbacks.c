// Copyright (C) 2024 Moritz Scheer

extern ngtcp2_callbacks callbacks = {
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





