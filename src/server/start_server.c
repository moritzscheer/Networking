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

	return server_loop(udp_socket);
}






