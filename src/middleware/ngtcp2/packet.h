// Copyright (C) 2024 Moritz Scheer

#ifndef _PACKET_H_
#define _PACKET_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define BUF_SIZE NGTCP2_MAX_UDP_PAYLOAD_SIZE


/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */



/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */



/* ------------------------------------------- TYPEDEF DECLARATIONS ------------------------------------------------- */



/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int verify_initial_packet(ngtcp2_token_type token_type, ngtcp2_cid *original_dcid, struct rqe *entry);


inline int send_connection_close_packet(uint32_t version, const ngtcp2_cid *dcid, const ngtcp2_cid *scid,
                                        uint64_t error_code, const uint8_t *reason, size_t reasonlen,
                                        struct sockaddr_storage *addr);

inline int send_retry_packet(uint32_t version, const ngtcp2_cid *dcid, const ngtcp2_cid *scid,
                             const ngtcp2_cid *original_dcid, const uint8_t *token, size_t tokenlen
                             struct sockaddr_storage *addr);

inline int send_stateless_reset_packet(ngtcp2_cid *dcid, struct sockaddr_storage *addr);

inline int send_version_negotiation_packet(const uint8_t *dcid, size_t dcidlen, const uint8_t *scid, size_t scidlen,
                                           struct sockaddr_storage *addr);
/* ------------------------------------------------------------------------------------------------------------------ */


#endif //_PACKET_H_
