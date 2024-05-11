//
// Created by Moritz on 5/11/24.
//

#ifndef _SERVER_H_
#define _SERVER_H_

typedef struct {
	int server_socket;
	struct sockaddr_storage local_addr;
	size_t local_addrlen;
	Connection *connections;
	ngtcp2_settings settings;
	ngtcp2_cid scid;
} Server;

#endif //_SERVER_H_
