// Copyright (C) 2024 Moritz Scheer

#ifndef _READ_H_
#define _READ_H_

int handle_read_result(server *server, request *request, int result);

int prepare_read(io_uring *ring, int socket, int *submissions, msghdr *message);

static int handle_read_error(message *message, int result);

#endif //_READ_H_
