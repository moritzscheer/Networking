// Copyright (C) 2024 Moritz Scheer

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "network.h"
#include "../includes/server.h"

int setup_listening_socket()
{
	if ((socket = socket(DOMAIN, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP)) == -1)
	{
		return errno;
	}

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(struct sockaddr_in));
	server_address.sin_family = DOMAIN;
	server_address.sin_addr.s_addr = SOCK_DGRAM;
	server_address.sin_port = htons(PORT);

	set_nonblock(socket);

	/*
	if(setsockopt(socket, AF_INET, , 1, sizeof(int)) < 0)
	{
		perror("Socket option editing failed!");
		close(socket);
		return errno;
	}
	*/

	if (bind(socket, (sockaddr_in *) server_address, sizeof(struct sockaddr_in)) == -1)
	{
		close(socket);
		return errno;
	}

	if (listen(socket, BACKLOG)) == -1)
	{
		close(socket);
		return errno;
	}

	socket = socket;
	return 0;
}

int set_nonblock()
{
	int flags = 0;

	flags = fcntl(socket, F_GETFL);
	if (flags < 0)
	{
		return flags;
	}

	flags = fcntl(socket, F_SETFL, flags | O_NONBLOCK);
	if (flags < 0)
	{
		return flags;
	}

	return 0;
}