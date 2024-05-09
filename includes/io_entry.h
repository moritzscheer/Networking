// Copyright (C) 2024 Moritz Scheer

#ifndef NETWORKING_ENTRY_H
#define NETWORKING_ENTRY_H

typedef struct {
    int event_type;
    struct sockaddr_in source_addr;
    int iovec_count;
    struct iovec iov[];
} io_entry;

#endif //NETWORKING_ENTRY_H
