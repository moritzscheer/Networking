// Copyright (C) 2024 Moritz Scheer

#ifndef _NETWORK_H_
#define _NETWORK_H_

int initialize_connection(int *server_socket, ngtcp2_settings settings);

int create_and_bind_socket(int *server_socket);

int setup_ngtcp2_server(ngtcp2_settings settings);

#endif //_NETWORK_H_
