// Copyright (C) 2024 Moritz Scheer

#ifndef _READ_H_
#define _READ_H_

#define MAX_TASKS 100

#include <sys/socket.h>
#include "../includes/server.h"

/* */
pthread_mutex_t read_mutex;

/* */
static struct queue new_packets = NULL;

/* */
static struct queue all_packets = NULL;

#define QD 64
#define BUF_SHIFT 12 /* 4k */
#define CQES (QD * 16)
#define BUFFERS CQES
#define CONTROLLEN 0

static struct io_uring *ring;
static struct io_uring_cqe *cqes;
static struct msghdr *msg;
static struct io_uring_recvmsg_out *msg_out;

/*
 *
 */
int listen_loop(struct server *server);

/*
 *
 */
static int setup_context(struct io_uring *ring, struct io_uring_cqe *cqe, struct msghdr *msg);

/*
 *
 */
static int cleanup(int result);

/*
 *
 */
static int prepare_read(io_uring *ring, int socket, struct msghdr *msg);

/*
 *
 */
static int get_sqe(struct io_uring *ring, struct io_uring_sqe **sqe);

/*
 *
 */
static int validate_cqe(struct io_uring_cqe *cqe);

/*
 *
 */
static int resolve_success(struct io_uring_cqe *cqe);

/*
 *
 */
static int resolve_error(struct server *server, struct msghdr *msg, int result);

/*
 *
 */
int fetch_read_message(struct msghdr *msg);

#endif //_READ_H_