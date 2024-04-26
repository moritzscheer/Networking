// Copyright (C) 2024 Moritz Scheer

#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int setup_socket(int domain, int type, int protocol, int port, )
{
	int server_socket;
	struct sockaddr_in server_address
	
	if(server.socket = socket(domain, type, protocol) < 0)
	{	
		perror("Socket creation failed!\n");
		exit(1);
	}
	
	/*
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(int)) < 0)
	{	
		perror("Socket option editing failed!\n");
		exit(1);
	}
	*/

	memset(&server_address, 0, sizeof(struct sockaddr_in));
	server_address.sin_family = domain;
	server_address.sin_addr.s_addr = type;
	server_address.sin_port = htons(port);
	
	if(bind(server.socket, (sockaddr_in*) &server.address, sizeof(server.address)) < 0)
	{	
		perror("Socket binding failed!\n");
		exit(1);
	}

	if(listen(server_socket, backlog)) < 0)
	{	
		perror("Socket listening failed!\n");
		exit(1);
	}

	return server_loop(server_socket);
}
