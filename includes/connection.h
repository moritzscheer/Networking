// Copyright (C) 2024 Moritz Scheer

#ifndef NETWORKING_CONNECTION_H
#define NETWORKING_CONNECTION_H

struct connection {
    uint8_t cid[LOCAL_CONN_ID_LEN];

    int client_socket;

    struct sockaddr_storage client_addr;
    socklen_t client_addr_len;

    quiche_conn *conn;

    UT_hash_handle hh;
};

#endif
