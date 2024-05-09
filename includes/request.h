// Copyright (C) 2024 Moritz Scheer

#ifndef SERVER
#define SERVER

typedef struct {
    int event_type;
    int iovec_count;
    struct iovec iov[];
} Request;

#endif


