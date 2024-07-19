// Copyright (C) 2024 Moritz Scheer

#ifndef _TOKEN_H_
#define _TOKEN_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */



/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */



/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */



/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int verify_token(ngtcp2_cid *dcid, ngtcp2_cid *original_dcid, ngtcp2_pkt_hd header,
                 ngtcp2_token_type token_type);

static inline int verify_retry_token(struct ngtcp2_cid *original_dcid, const struct ngtcp2_pkt_hd *header,
                                     const struct sockaddr *sockaddr, socklen_t sockaddr_len);

static inline int verify_token(const struct ngtcp2_pkt_hd *header,
                               const struct sockaddr *sockaddr, socklen_t sockaddr_len);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_TOKEN_H_
