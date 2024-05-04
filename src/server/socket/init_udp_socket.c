// Copyright (C) 2024 Moritz Scheer

#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "../types.h"
#include <stdlib.h>

int init_udp_socket(struct *udp_env)
{
    if((udp_env->socket = socket(DOMAIN, SOCK_DGRAM, IPPROTO_QUIC)) < 0)
    {
        perror("Socket creation failed!\n");
        exit(1);
    }

    memset(udp_env->address, 0, sizeof(struct sockaddr_in));
    udp_env->address.sin_family = DOMAIN;
    udp_env->address.sin_addr.s_addr = SOCK_DGRAM;
    udp_env->address.sin_port = htons(PORT);

    if(setsockopt(udp_env->socket, SOL_QUIC, QUIC_SOCKOPT_ALPN, 1, sizeof(int))) < 0)
    {
        perror("Socket option editing failed!\n");
        exit(1);
    }
    if (quic_server_handshake(sockfd, argv[4], argv[5]))
    {
        perror("Socket option editing failed!\n");
        exit(1);
    }

    if(bind(udp_env->socket, (sockaddr_in*) udp_env->address, sizeof(struct sockaddr_in)) < 0)
    {
        perror("Socket binding failed!\n");
        exit(1);
    }

    if(listen(server_socket, BACKLOG)) < 0)
    {
        perror("Socket listening failed!\n");
        exit(1);
    }

    return;
}
