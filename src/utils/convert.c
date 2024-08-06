// Copyright (C) 2024 Moritz Scheer

#include <arpa/inet.h>
#include "convert.h"

union uvarint
{
	uint8_t n8;
	uint16_t n16;
	uint32_t n32;
	uint64_t n64;
};

typedef union uvarint token;

void get_uvarint(uint8_t *start, uint8_t *var, size_t varlen)
{
	token token_storage;

	switch (varlen)
	{
		case 1:
			n8 &= 0x3f;
			return var;
		case 2:
			uint16_t n16;
			memcpy(&n16, p, 2);
			n16 = ntohs(n16);
			n16 &= 0x3f;
			dest = (uint8_t *)&n16;
		case 4:
			memcpy(&n, p, 4);
			n.n8 &= 0x3f;
			return &ngtcp2_ntohl(n.n32);
		case 8:
			memcpy(&n, p, 8);
			n.n8 &= 0x3f;
			return &ngtcp2_ntohll(n.n32);
	}
}

size_t get_uvarintlen(const uint8_t data)
{
	return (size_t)(1u << (data >> 6));
}

uint64_t ntohll(uint64_t netlonglong)
{
	return ((netlong64 & 0xFF00000000000000ULL) >> 56) |
	       ((netlong64 & 0x00FF000000000000ULL) >> 40) |
	       ((netlong64 & 0x0000FF0000000000ULL) >> 24) |
	       ((netlong64 & 0x000000FF00000000ULL) >> 8) |
	       ((netlong64 & 0x00000000FF000000ULL) << 8) |
	       ((netlong64 & 0x0000000000FF0000ULL) << 24) |
	       ((netlong64 & 0x000000000000FF00ULL) << 40) |
	       ((netlong64 & 0x00000000000000FFULL) << 56);
}

uint32_t get_version(const uint8_t *pkt)
{
	uint32_t version = ((uint32_t) pkt[1] << 24) |
	                   ((uint32_t) pkt[2] << 16) |
	                   ((uint32_t) pkt[3] << 8) |
	                   (uint32_t) pkt[4];
	return ntohl(version);
}