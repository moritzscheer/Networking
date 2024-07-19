// Copyright (C) 2024, Moritz Scheer

#ifndef _RING_H_
#define _RING_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define SQES 64 // num of

#define BUFFER_SHIFT 12 // 4KB

#define CQES (SQES * 16) // num of completion entries

#define CONTROLLEN 0

#define BUFFERS CQES

#define BUFFER_SIZE (1U << BUFFER_SHIFT)

#define BUFFER_RING_SIZE (sizeof(struct io_uring_buf) + BUFFER_SIZE) * BUFFERS

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

extern struct io_uring ring;

extern struct msghdr msg;

extern struct io_uring_cqe *cqes;

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_RING_H_
