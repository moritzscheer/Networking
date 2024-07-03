// Copyright (C) 2024 Moritz Scheer

#ifndef _READ_H_
#define _READ_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define MAX_TASKS 100

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

static int socket;

static struct io_uring *ring;

static struct io_uring_cqe *cqes;

static struct msghdr *msg;

static struct io_uring_recvmsg_out *msg_out;

static struct pkt *prev_pkt = NULL;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int listen_loop(struct server *server);

static int prepare_sqe(int socket);

static inline int validate_cqe(struct io_uring_cqe *cqe);

static int resolve_success(struct io_uring_cqe *cqe);

static int resolve_error(struct server *server, struct msghdr *msg, int result);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_READ_H_