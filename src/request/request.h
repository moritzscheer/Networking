// Copyright (C) 2024 Moritz Scheer
#ifndef SERVER
#define SERVER

typedef struct request {
    int event_type;
    int iovec_count;
    int client_socket;
    struct iovec iov[];
} Request;

#endif


