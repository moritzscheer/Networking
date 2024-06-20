// Copyright (C) 2024, Moritz Scheer

#ifndef _RESPONSE_H_
#define _RESPONSE_H_

int get_version_negotiation_packet();
int get_retry_packet();
int get_connection_close_packet();
int get_stateless_reset_packet();

#endif //_RESPONSE_H_
