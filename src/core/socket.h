// Copyright (C) 2024 Moritz Scheer

#ifndef _SOCKET_H_
#define _SOCKET_H_

int initialize_socket(int *socket);
struct msghdr *init_msghdr();
struct msghdr *init_cmsghdr(struct msghdr *msg, size_t data_len);
struct msghdr *init_iovec(struct msghdr *msg, size_t buf_size, size_t num_iov);

#endif //_SOCKET_H_
