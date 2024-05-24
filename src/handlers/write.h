// Copyright (C) 2024 Moritz Scheer

#ifndef _WRITE_H_
#define _WRITE_H_

int handle_write_result(server *server, io_buffer *io_buffer, int result);
int prepare_write(io_uring *ring, int socket, int *submissions, msghdr *message);

#endif //_WRITE_H_
