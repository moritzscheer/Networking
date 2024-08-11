// Copyright (C) 2024 Moritz Scheer

#ifndef _CONVERT_H_
#define _CONVERT_H_

union uvarint
{
	uint8_t n8;
	uint16_t n16;
	uint32_t n32;
	uint64_t n64;
};

size_t get_uvarint(uint8_t *start, union uvarint *var)

uint64_t ntohll(uint64_t netlong64);

uint32_t get_version(const uint8_t *pkt);

#endif //_CONVERT_H_
