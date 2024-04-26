// Copyright (C) 2024 Moritz Scheer

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <liburing.h>
#include "../request.h"

int add_accept_request(int server_socket, struct sockaddr_in *client_addr, socklen_t *client_addr_len) 
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	io_uring_prep_accept(sqe, server_socket, (struct sockaddr *) client_addr, client_addr_len, 0);

	// create Request and set type to accept
	Request *client_request = malloc(sizeof(struct request));
	client_request->event_type = EVENT_TYPE_ACCEPT;
	
	io_uring_sqe_set_data(sqe, client_request);
	io_uring_submit(&ring);
	
	return;
}
