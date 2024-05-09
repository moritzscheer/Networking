// Copyright (C) 2024, Moritz Scheer

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <liburing.h>
#include <string.h>
#include <stdint.h>
#include <sys/uio.h>
#include "../../includes/request.h"
#include "../utils/error/p_return.c"

#define READ =      0
#define WRITE =     1
#define CLOSE =     2
#define BUFSIZE = 65535 
static const size_t REQUEST_SIZE = sizeof(struct request) + sizeof(struct iovec);

void add_read_request(io_uring *ring, int server_socket)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    if (!sqe) p_return("Failed to get SQE")

    struct request *request = malloc(REQUEST_SIZE);
    if (!request) p_return("Failed to allocate buffer for request struct")

    uint8_t *buffer = malloc(BUFSIZE)
    if (!buffer) p_return("Failed to allocate buffer for iov_base")
    memset(buffer, 0, BUFSIZE);

    request->iov[0].iov_base = buffer;
    request->iov[0].iov_len = BUFSIZE;
    request->event_type = READ;

    io_uring_prep_read(sqe, server_socket, request->iov, request->iov[0].iov_len, 0);
    io_uring_sqe_set_data(sqe, request);
}

void add_write_request(io_uring *ring, Response *response)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    if (!sqe) p_return("Failed to get SQE")

    response->event_type = WRITE;

    io_uring_prep_writev(sqe, response->client_socket, response->iov, response->iovec_count, 0);
    io_uring_sqe_set_data(sqe, response);
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
