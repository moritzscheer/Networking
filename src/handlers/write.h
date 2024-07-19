// Copyright (C) 2024 Moritz Scheer

#ifndef _WRITE_H_
#define _WRITE_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define WRITE 1

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int prepare_write(struct io_uring *ring, int socket, struct msghdr *msg);

inline int validate_write(struct io_uring_cqe *cqe);

static inline int resolve_success();

static inline int resolve_write(int socket, int result);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_WRITE_H_
