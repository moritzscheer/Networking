// Copyright (C) 2024 Moritz Scheer

#include <liburing.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../request/types/read_request.c"
#include "../../request/types/accept_request.c"
#include "../../request/hamdle/handle_request.c"
#include "../../request/request.h"
#include "../../utils/error/fatal_error.c"

void handle_connection(int server_socket)
{
	struct io_uring_cqe *cqe;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	accept_request(server_socket, client_addr, client_addr_len);

	while(1)
	{
		int cqe_ptr;
		if(cqe_ptr = io_uring_wait_cqe(&ring, &cqe) < 0)
			fatal_error("Waiting for an IO completion failed!\n");
		
		Request *req = (struct request *) cqe->user_data;
		
		if (cqe->res < 0) 
			fatal_error("Async request failed!\n");

		switch (req->event_type) {
			case EVENT_TYPE_ACCEPT:
				accept_request(server_socket, &client_addr, &client_addr_len);
				read_request(cqe->res);
				break;

			case EVENT_TYPE_READ:
				if (!cqe->res) 
				{
					fprintf(stderr, "Empty request!\n");
					break;
				}
				handle_request(req);
				free(req->iov[0].iov_base);
				break;

			case EVENT_TYPE_WRITE:
				for (int i = 0; i < req->iovec_count; i++) {
					free(req->iov[i].iov_base);
				}
				close(req->client_socket);
				break;
		}
		io_uring_cqe_seen(&ring, cqe);
		free(req);
	}
}
