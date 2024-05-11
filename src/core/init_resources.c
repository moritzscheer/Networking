// Copyright (C) 2024 Moritz Scheer

#include "./init_socket.h"

int init_resources(Server server)
{
	int server_socket = init_socket();

	pthread_t *threads = init_threads(udp_socket);

}