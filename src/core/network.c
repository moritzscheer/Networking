// Copyright (C) 2024 Moritz Scheer

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <linux/types.h>
#include <linux/io_uring.h>
#include <liburing.h>

#include "network.h"
#include "../includes/server.h"

int initialize_connection(Server *server)
{
	int res;
	start_step("Establish server connection");

	res = create_and_bind_socket(server->socket);
	if (res != 0)
	{
		return end_step("Failed to establish server connection", res);
	}

	res = io_uring_queue_init(QUEUE_DEPTH, server->ring, 0);
	if (res != 0)
	{
		return end_step("Failed to establish server connection", errno);
	}

	res = setup_ngtcp2_settings(server->settings);
	if (res != 0)
	{
		return end_step("Failed to establish server connection", res);
	}

	return end_step("Server connection established", 0);
}

int create_and_bind_socket(int *server_socket)
{
	struct sockaddr_in server_address;

	if ((*server_socket = socket(DOMAIN, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP)) == -1)
	{
		perror("Socket creation failed!");
		return errno;
	}

	memset(&server_address, 0, sizeof(struct sockaddr_in));
	server_address.sin_family = DOMAIN;
	server_address.sin_addr.s_addr = SOCK_DGRAM;
	server_address.sin_port = htons(PORT);

	/*
	if(setsockopt(*server_socket, AF_INET, , 1, sizeof(int)) < 0)
	{
		perror("Socket option editing failed!");
		close(*server_socket);
		return errno;
	}
	*/

	if (bind(*server_socket, (sockaddr_in *) server_address, sizeof(struct sockaddr_in)) == -1)
	{
		perror("Socket binding failed!");
		close(*server_socket);
		return errno;
	}

	if (listen(*server_socket, BACKLOG)) == -1)
	{
		perror("Socket listening failed!");
		close(*server_socket);
		return errno;
	}

	return 0;
}

int setup_ngtcp2_server(ngtcp2_settings settings)
{
	ngtcp2_settings_default(&server.settings);
	server.settings.initial_ts = timestamp();
	server.settings.log_printf = log_printf;

	return 0;
}

int init_ngtcp2_callbacks()
{
	ngtcp2_callbacks callbacks = {
		/*  used callback function                      default callback functions */
		nullptr,                                    /* client_initial */
		ngtcp2_crypto_recv_client_initial_cb,       /* recv_client_initial */
		recv_crypto_data,                           /* recv_crypto_data */
		handshake_completed,                        /* handshake_completed */
		nullptr,                                    /* recv_version_negotiation */
		ngtcp2_crypto_encrypt_cb                    /* encrypt */
		ngtcp2_crypto_decrypt_cb,                   /* decrypt */
		do_hp_mask,                                 /* hp_mask */
		recv_stream_data,                           /* recv_stream_data */
		acked_stream_data_offset,                   /* acked_stream_data_offset */
		stream_open,                                /* stream_open */
		stream_close,                               /* stream_close */
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
}