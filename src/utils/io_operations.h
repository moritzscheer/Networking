// Copyright (C) 2024, Moritz Scheer

#ifndef _IO_OPERATIONS_H_
#define _IO_OPERATIONS_H_

#define READ =      0
#define WRITE =     1
#define CLOSE =     2
#define BUFFER_SIZE = 65535

static const size_t REQUEST_SIZE = sizeof(struct request) + sizeof(struct iovec);

void prep_read_io(io_uring *ring, int server_socket);
void prep_write_io(io_uring *ring, IORequest *request);
void add_close_request(io_uring *ring, IORequest *request);

#endif //_IO_OPERATIONS_H_
