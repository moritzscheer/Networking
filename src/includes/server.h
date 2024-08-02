// Copyright (C) 2024 Moritz Scheer

#ifndef _SERVER_H_
#define _SERVER_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define MAX_TRIES 3

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

struct secret
{
	size_t datalen;
	uint8_t *data;
};

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

struct sockaddr_storage local_addr;

size_t local_addrlen;

struct secret secret;

int listen_socket;

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_SERVER_H_