// Copyright (C) 2024 Moritz Scheer

#include <arpa/inet.h>
#include "convert.h"

size_t get_uvarint(uint8_t *start, union uvarint *var)
{
	size_t varlen = (size_t)(1u << (start >> 6));

	switch (varlen)
	{
		case 1:
			memcpy(var, start, 1);
			break;
		case 2:
			memcpy(var, start, 2);
			var->n8 &= 0x3f;
			ngtcp2_ntohs(var->n16);
			break;
		case 4:
			memcpy(var, start, 4);
			var->n8 &= 0x3f;
			ntohl(var->n32);
			break;
		case 8:
			memcpy(var, start, 8);
			var->n8 &= 0x3f;
			ntohll(var->n64);
			break;
	}

	return varlen;
}

size_t get_uvarintlen(const uint8_t *start)
{
	return (size_t)(1u << (*start >> 6));
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