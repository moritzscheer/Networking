// Copyright (C) 2024 Moritz Scheer

Response *parse_request(Connection *connections, Request *request)
{
    uint8_t buf[BUF_SIZE];

    for (1)
    {
        uint32_t version;
        const uint8_t *dcid, *scid;
        size_t dcidlen, scidlen;

        ret = ngtcp2_pkt_decode_version_cid(&version, &dcid, &dcidlen, &scid, &scidlen, buf, n_read, NGTCP2_MAX_CIDLEN);
        if (ret < 0)
            p_return("ngtcp2_pkt_decode_version_cid: %s", ngtcp2_strerror(ret));

        Connection *connection = find_connection(server, dcid, dcidlen);
        if (!connection)
        {
            connection = create_connection();
            if (!connection)
            {
                return -1;
            }
        }

        ngtcp2_path path;
        memcpy(&path, ngtcp2_conn_get_path(connection->conn), sizeof(path));
        path.remote.addrlen = remote_addrlen;
        path.remote.addr = (struct sockaddr*) &remote_addr;

        ngtcp2_pkt_info pi;
        memset(&pi, 0, sizeof(pi));

        ret = ngtcp2_conn_read_pkt (conn, &path, &pi, buf, n_read, timestamp ());
        if (ret == NGTCP_ERR_RETRY)
        {
            HASH_DEL(connections, connection);
            free_connection(connection);
        }
        else if
        {

        }
        else if
        {

        }
        else if
        {

        }
        else
        {

        }
    }
    return 0;
}