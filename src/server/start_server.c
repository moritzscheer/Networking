// Copyright (C) 2024 Moritz Scheer

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/quic.h>

static void server_loop(int server_socket);

int start_server(void)
{

	int tcp_socket = setup_socket(DOMAIN, SOCK_STREAM, IPPROTO_TCP, PORT);

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

	return server_loop(server_socket);
}



static void server_loop(int tcp_socket, int quic_socket)
{
	struct io_uring ring;
	struct io_uring_cqe *cqe;
	struct sockaddr_in client_addr;

	socklen_t client_addr_len = sizeof(client_addr);
	
	if(io_uring_queue_init(QUEUE_DEPTH, &ring, 0) < 0)
	{
		perror("Could not initialize io_uring queue.");
		exit(1);
	}

	add_accept_request(tcp_socket, &client_addr, &client_addr_len);
	add_accept_request(quic_socket, &client_addr, &client_addr_len);

	while(1) 
	{
		int ret = io_uring_wait_cqe(&ring, &cqe); 
		if(ret < 0) fatal_error("io_uring_wait_cqe");

		Request *req = (Request *) cqe->user_data;
		if (cqe->res < 0) {
                    fprintf(stderr, "Async request failed: %s for event: %d\n",
                    strerror(-cqe->res), req->event_type);
                    exit(1);
                }

	        switch (req->event_type) {
                    case EVENT_TYPE_ACCEPT:
                        add_accept_request(server_socket, &client_addr, &client_addr_len);
                        add_read_request(cqe->res);
                        free(req);
                        break;
                    case EVENT_TYPE_READ:
                        if (!cqe->res) {
                            fprintf(stderr, "Empty request!\n");
                            break;
                        }
                        handle_client_request(req);
                        free(req->iov[0].iov_base);
                        free(req);
                        break;
                    case EVENT_TYPE_WRITE:
                        for (int i = 0; i < req->iovec_count; i++) {
                            free(req->iov[i].iov_base);
                        }
                        close(req->client_socket);
                        free(req);
                        break;
                    default:
                        fprintf(stderr, "Unexpected req type %d\n", req->event_type);
                        break;
                }
	        io_uring_cqe_seen(&ring, cqe);
        }
	io_uring_queue_exit(&ring);
}


