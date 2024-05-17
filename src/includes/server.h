// Copyright (C) 2024 Moritz Scheer

#ifndef _SERVER_H_
#define _SERVER_H_

typedef struct server {
	int socket;

	struct sockaddr_storage local_addr;
	size_t local_addrlen;

	Connection *connections;

	ngtcp2_settings settings;
	ngtcp2_cid scid;
} server;

#endif //_SERVER_H_