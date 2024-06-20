// Copyright (C) 2024 Moritz Scheer

#ifndef _SERVER_H_
#define _SERVER_H_

struct server
{
	/* Socket descriptor for the server */
	int socket;

	/* Local address information */
	struct sockaddr_storage local_addr;
	size_t local_addrlen;

	/* Hash table of active connections */
	struct connection *connections;
	UT_hash_handle hh;

	/* QUIC settings and connection ID (CID) */
	ngtcp2_settings *settings;
	ngtcp2_cid *scid;
};

#endif //_SERVER_H_