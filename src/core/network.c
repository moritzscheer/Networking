// Copyright (C) 2024 Moritz Scheer

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "network.h"
#include "../includes/server.h"
#include "../includes/status.h"

int setup_listening_socket(void)
{
	listen_socket = socket(AF_INET6, SOCK_DGRAM, 0);
	if (listen_socket == -1)
	{
		return errno;
	}

	int dual_stack = 0;
	if(setsockopt(listen_socket, IPPROTO_IPV6, IPV6_V6ONLY, &dual_stack, sizeof(dual_stack)) < 0)
	{
		close(listen_socket);
		return errno;
	}

	res = set_nonblock(listen_socket);
	if(res != 0)
	{
		close(listen_socket);
		return res;
	}

	struct sockaddr_in6 addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin6_family = AF_INET6;
	addr.sin6_addr = in6addr_any;
	addr.sin6_port = htons(PORT);

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

int set_nonblock(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1)
	{
		return errno;
	}

	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		return errno;
	}
	return 0;
}

socklen_t get_addrlen(struct sockaddr_storage *addr)
{
	switch (addr->ss_family)
	{
		case AF_INET:
		{
			return sizeof(struct sockaddr_in);
		}
		case AF_INET6:
		{
			return sizeof(struct sockaddr_in6);
		}
		case AF_UNIX:
		{
			return sizeof(struct sockaddr_un);
		}
		default:
		{
			return 0;
		}
	}
}