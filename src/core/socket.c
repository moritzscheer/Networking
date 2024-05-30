// Copyright (C) 2024 Moritz Scheer

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "socket.h"
#include "../../config/server_conf.h"

int initialize_connection(int *server_socket, ngtcp2_settings settings)
{
	start_step("Establish server connection");

	int res = create_and_bind_socket(server_socket);
	if (res != 0)
	{
		return end_step("Failed to establish server connection", res);
	}

	res = setup_ngtcp2_settings(settings);
	if (res != 0)
	{
		return end_step("Failed to establish server connection", res);
	}

	return end_step("Server connection established", 0);
}

int create_and_bind_socket(int *server_socket)
{
	struct sockaddr_in server_address;

	if((*server_socket = socket(DOMAIN, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP)) == -1)
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

	if(bind(*server_socket, (sockaddr_in*) server_address, sizeof(struct sockaddr_in)) == -1)
	{
		perror("Socket binding failed!");
		close(*server_socket);
		return errno;
	}

	if(listen(*server_socket, BACKLOG)) == -1)
	{
		perror("Socket listening failed!");
		close(*server_socket);
		return errno;
	}

	return 0;
}

int setup_ngtcp2_server(ngtcp2_settings settings)
{
	return 0;
}