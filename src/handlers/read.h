// Copyright (C) 2024 Moritz Scheer

#ifndef _READ_H_
#define _READ_H_

#define MAX_IOV         10
#define REQUEST_SIZE    69069
#define BUFFER_SIZE         4096

int handle_read_result(server *server, request *request, int result);
int *prepare_read(io_uring *ring, int socket, io_buffer *io_buffer);

#endif //_READ_H_
