// Copyright (C) 2024 Moritz Scheer

#ifndef NETWORKING_TYPES_H
#define NETWORKING_TYPES_H

typedef struct socket {
    socket: int;
    struct sockaddr_in address;
    threads: int*;
} socket;

#endif //NETWORKING_TYPES_H
