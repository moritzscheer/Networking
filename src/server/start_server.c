// Copyright (C) 2024 Moritz Scheer

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/quic.h>
#include <stdlib.h>
#include "server_loop.c"
#include "types.h"

int start_server(void)
{
    socket *udp_socket = malloc(sizeof(struct udp_env));
    init_udp_socket(udp_socket);

    pthread_t* threads = init_threads(udp_socket);

	if(setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(int)) < 0)
	{
		perror("Socket option editing failed!\n");
		exit(1);
	}

	int quic_socket = setup_socket(DOMAIN, SOCK_DGRAM, IPPROTO_QUIC, PORT);

	if(setsockopt(server_socket, SOL_QUIC, QUIC_SOCKOPT_ALPN, 1, sizeof(int))) < 0)
	{
		perror("Socket option editing failed!\n");
		exit(1);
	}

	if (quic_server_handshake(sockfd, argv[4], argv[5]))
	{
		perror("Socket option editing failed!\n");
		exit(1);
	}
	
	printf("server has been created.");
	return server_loop(udp_socket);
}

