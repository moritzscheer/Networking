// Copyright (C) 2024 Moritz Scheer

#ifndef SERVER
#define SERVER

typedef struct {
    int event_type;
    struct sockaddr_in source_addr;
    int iovec_count;
    struct iovec iov[];
} Request;

#endif


