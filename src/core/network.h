// Copyright (C) 2024 Moritz Scheer

#ifndef _NETWORK_H_
#define _NETWORK_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */



/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */



/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */



/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int initialize_connection(int *server_socket, ngtcp2_settings settings);

int setup_listening_socket(int *server_socket);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_NETWORK_H_
