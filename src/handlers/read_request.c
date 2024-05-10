// Copyright (C) 2024, Moritz Scheer

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