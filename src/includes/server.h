// Copyright (C) 2024 Moritz Scheer

#ifndef _SERVER_H_
#define _SERVER_H_

typedef struct Server {
	int socket;

	struct sockaddr_storage local_addr;
	size_t local_addrlen;

	Connection *connections;
	Bufferpool *buffers;

	ngtcp2_settings settings;
	ngtcp2_cid scid;
} Server;

#endif //_SERVER_H_