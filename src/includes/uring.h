// Copyright (C) 2024 Moritz Scheer

#ifndef _URING_H_
#define _URING_H_

typedef struct uring {
	struct io_uring *ring;
	struct io_uring_cqe *cqe;
} uring;

#endif //_URING_H_