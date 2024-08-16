// Copyright (C) 2024 Moritz Scheer

#ifndef _HEADER_H_
#define _HEADER_H_

#define FIXED_BIT_MASK 0x40
#define FORM_BIT_MASK 0x80

#define TYPE_BIT_MASK 0x30

#define PKT_INITIAL 0x00
#define PKT_0RTT 0x01
#define PKT_HANDSHAKE 0x02
#define PKT_RETRY 0x03

#define SHORT_HEADER_PKT_MIN_LENGTH 1 + NGTCP2_MAX_CIDLEN // 1 + max cid bytes

#define LONG_HEADER_PKT_MIN_LENGTH 7 // 7 bytes

static struct connection *connection;

static int decode_header(uint8_t *pkt, size_t pktlen, struct sockaddr_storage *addr, socklen_t addrlen);

static int decode_short_header(struct read_storage *event, uint8_t *pkt, size_t pktlen, struct sockaddr_storage *addr,
                               socklen_t addrlen);

static int decode_long_header(struct read_storage *event, void *pkt, size_t pktlen, struct sockaddr_storage *addr,
                              socklen_t addrlen);

#endif //_HEADER_H_
