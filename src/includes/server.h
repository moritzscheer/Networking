// Copyright (C) 2024 Moritz Scheer

#ifndef _SERVER_H_
#define _SERVER_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define MAX_TRIES 3

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

struct server
{
	/* Local address information */
	struct sockaddr_storage local_addr;
	size_t local_addrlen;

	/* Hash table of active connections */
	struct connection *connections;
	UT_hash_handle hh;

	ngtcp2_cid *scid;
};

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

struct server *server;

int socket;

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_SERVER_H_