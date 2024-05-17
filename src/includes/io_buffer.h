// Copyright (C) 2024, Moritz Scheer

#ifndef _REQUEST_H_
#define _REQUEST_H_

#define READ 0
#define WRITE 1
#define CLOSE 2

typedef struct io_buffer {
	int event_type;
	struct sockaddr_in source_addr;
	int iovec_count;
	struct iovec iov[];
	int tries;
} io_buffer;

#endif //_REQUEST_H_
