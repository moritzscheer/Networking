// Copyright (C) 2024, Moritz Scheer

void prep_write_io(io_uring *ring, Response *response)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    if (!sqe) p_return("Failed to get SQE")

    response->event_type = WRITE;

    io_uring_prep_writev(sqe, response->client_socket, response->iov, response->iovec_count, 0);
    io_uring_sqe_set_data(sqe, response);
}