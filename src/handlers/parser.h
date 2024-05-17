// Copyright (C) 2024 Moritz Scheer

#ifndef _PARSER_H_
#define _PARSER_H_

int parse(Connection *connections, io_buffer *io_buffer);
int decode(iovec iov, *version, *dcid, *dcidlen, *scid, *scidlen);

#endif //_PARSER_H_
