// Copyright (C) 2024 Moritz Scheer

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <liburing.h>
#include <string.h>
#include "../../includes/request.h"

// event_type definitions
#define READ =      0
#define WRITE =     1
#define CLOSE =     2

void add_read_request(io_uring *ring, int client_socket)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    if (!sqe)
    {
        perror("Failed to get SQE");
        return;
    }

    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    uint8_t buf[65535];

    Request *request = malloc(sizeof(Request) + sizeof(struct iovec));
    if (!request)
    {
        perror("Failed to allocate buffer");
        return;
    }
    memset(&request, 0, sizeof(request));

    request->event_type = READ;
    request->client_addr = client_addr;
    request->client_addr_len = client_addr_len;
    request->iov[0].iov_base = malloc(buf);
    request->iov[0].iov_len = sizeof(buf);

    if (!request->iov[0].iov_base)
    {
        perror("Failed to allocate buffer");
        return;
    }

    io_uring_prep_recv(sqe, client_socket, request->iov[0].iov_base, request->iov[0].iov_len, 0);
    io_uring_sqe_set_data(sqe, request);
}

void add_write_request(io_uring *ring, Response *response)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);

    response->event_type = WRITE;

    io_uring_prep_writev(sqe, response->client_socket, response->iov, response->iovec_count, 0);
    io_uring_sqe_set_data(sqe, request);
}

void add_close_request(io_uring *ring, Request *request)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_close(sqe, request->client_socket);

    request->event_type = CLOSE;

    io_uring_sqe_set_data(sqe, request);
}

void free_request(Request *request)
{
    if (request->iov) {
        free(request->iov->iov_base);
        free(request->iov);
    }
    free(request);
}

void free_response(Response *response) {
    if (response->headers) {
        free(response->headers);
    }
    if (response->body) {
        free(response->body);
    }
    free(response);
}