// Copyright (C) 2024, Moritz Scheer

#ifndef _IOREQUEST_H_
#define _IOREQUEST_H_

typedef struct {
	int event_type;
	struct sockaddr_in source_addr;
	int iovec_count;
	struct iovec iov[];
} IORequest;

#endif //_IOREQUEST_H_
