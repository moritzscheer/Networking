// Copyright (C) 2024 Moritz Scheer

#ifndef _URING_H_
#define _URING_H_

typedef struct Uring {
	struct io_uring *ring;
	struct io_uring_cqe *cqe;
} Uring;

#endif //_URING_H_