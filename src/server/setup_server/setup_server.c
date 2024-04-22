// Copyright (C) 2024 Moritz Scheer

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include "./server.h"
#include "../../utils/error/fatal_error.c"

int setup_server(void)
{
	int server_socket;
	struct sockaddr_in server_address

	if(server.socket = socket(DOMAIN, TYPE, PROTOCOL) < 0)
		fatal_error("Socket creation failed!\n");

	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(int)) < 0)
		fatal_error("Socket option editing failed!\n");

	memset(&server_address, 0, sizeof(struct sockaddr_in));
	server_address.sin_family = DOMAIN;
	server_address.sin_addr.s_addr = TYPE;
	server_address.sin_port = htons(PORT);

	if(bind(server.socket, (sockaddr_in*) &server.address, sizeof(server.address)) < 0)
		fatal_error("Socket binding failed!\n");

	if(listen(server_socket, backlog)) < 0)
		fatal_error("Socket listening failed!\n");

	return server_socket;
}


