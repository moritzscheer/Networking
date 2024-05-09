// Copyright (C) 2024 Moritz Scheer

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/quic.h>
#include <stdlib.h>
#include "server_loop.c"
#include "init_socket.c"
#include "init_threads.c"


int start_server(void)
{
    int server_socket = init_socket();

	pthread_t* threads = init_threads(udp_socket);

    server_loop(server_socket);

	return handle_shutdown(server_socket, threads);
}

