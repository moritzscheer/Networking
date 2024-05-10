// Copyright (C) 2024 Moritz Scheer

#include "../includes/connection.h"

Connection create_connection(SSL session, int socket_fd)
{

}

void close_connection()
{

}

void free_connection()
{

}

int read_connection()
{

}

int write connection()
{

}

Connection *find_connection(Connection* connections, uint8_t *dcid, size_t dcid_size)
{
    Connection *connection;

    for (connection = connections; connection != NULL; connection = connection->hh.next)
    {
        size_t n_scids = ngtcp2_conn_get_scid(connection->conn, NULL);

        ngtcp2_cid *scids = malloc(sizeof(ngtcp2_cid) * n_scids);
        if(!scids) return NULL;
        ngtcp2_conn_get_scid(connection->conn, scids);

        for (size_t i = 0; i < n_scids; i++)
        {
            if (dcid_size == scids[i].datalen &&
                memcmp(dcid, scids[i].data, dcid_size) == 0)
            {
                free(scids);
                return connection;
            }
        }
        free(scids);
    }
    return NULL;
}