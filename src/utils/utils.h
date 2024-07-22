// Copyright (C) 2024 Moritz Scheer

#ifndef _UTILS_H_
#define _UTILS_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */



/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */



/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */



/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

ngtcp2_tstamp get_timestamp_ns();

int get_random(void *data, size_t datalen);

int get_random_cid(ngtcp2_cid *cid, size_t len);

ngtcp2_cid *generate_cid();

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_UTILS_H_
