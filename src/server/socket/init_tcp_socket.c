// Copyright (C) 2024 Moritz Scheer

#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "../types.h"
#include <stdlib.h>

int init_tcp_socket(struct *tcp_env)
{
    if((tcp_env->socket = socket(DOMAIN, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("Socket creation failed!\n");
        exit(1);
    }

    memset(tcp_env->address, 0, sizeof(struct sockaddr_in));
    tcp_env->address.sin_family = DOMAIN;
    tcp_env->address.sin_addr.s_addr = SOCK_STREAM;
    tcp_env->address.sin_port = htons(PORT);

    if(setsockopt(tcp_env->socket, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(int)) < 0)
    {
        perror("Socket option editing failed!\n");
        exit(1);
    }

    if(bind(tcp_env->socket, (sockaddr_in*) tcp_env->address, sizeof(struct sockaddr_in)) < 0)
    {
        perror("Socket binding failed!\n");
        exit(1);
    }

    if(listen(tcp_env->socket, BACKLOG)) < 0)
    {
        perror("Socket listening failed!\n");
        exit(1);
    }

    return;
}