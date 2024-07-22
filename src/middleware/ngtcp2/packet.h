// Copyright (C) 2024 Moritz Scheer

#ifndef _PACKET_H_
#define _PACKET_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define BUF_SIZE NGTCP2_MAX_UDP_PAYLOAD_SIZE


/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */



/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */



/* ------------------------------------------- TYPEDEF DECLARATIONS ------------------------------------------------- */



/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

inline int send_connection_close_packet(uint32_t version, const ngtcp2_cid *dcid, const ngtcp2_cid *scid,
                                        uint64_t error_code, const uint8_t *reason, size_t reasonlen);

inline int send_retry_packet(uint32_t version, const ngtcp2_cid *dcid, const ngtcp2_cid *scid, const ngtcp2_cid *odcid,
                             const uint8_t *token, size_t tokenlen);

inline int send_stateless_reset_packet(const uint8_t *stateless_reset_token);

inline int send_version_negotiation_packet(const uint8_t *dcid, size_t dcidlen, const uint8_t *scid, size_t scidlen);

/* ------------------------------------------------------------------------------------------------------------------ */


#endif //_PACKET_H_
