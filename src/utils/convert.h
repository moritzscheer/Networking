// Copyright (C) 2024 Moritz Scheer

#ifndef _CONVERT_H_
#define _CONVERT_H_

uint64_t get_uvarint(const uint8_t *data, size_t *datalen);

size_t get_uvarintlen(const uint8_t *start);

uint64_t ntohll(uint64_t netlong64);

uint32_t get_version(const uint8_t *pkt);

#endif //_CONVERT_H_
