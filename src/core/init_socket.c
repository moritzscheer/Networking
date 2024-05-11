// Copyright (C) 2024 Moritz Scheer

#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int init_udp_socket()
{
    int server_socket;
    struct sockaddr_in server_address;

    if((server_socket = socket(DOMAIN, SOCK_DGRAM, IPPROTO_QUIC)) < 0)
    {
        perror("Socket creation failed!\n");
        exit(1);
    }

    memset(server_address, 0, sizeof(struct sockaddr_in));
    server_address.sin_family = DOMAIN;
    server_address.sin_addr.s_addr = SOCK_DGRAM;
    server_address.sin_port = htons(PORT);

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

    if(bind(server_socket, (sockaddr_in*) server_address, sizeof(struct sockaddr_in)) < 0)
    {
        perror("Socket binding failed!\n");
        exit(1);
    }

    if(listen(server_socket, BACKLOG)) < 0)
    {
        perror("Socket listening failed!\n");
        exit(1);
    }

    return server_socket;
}