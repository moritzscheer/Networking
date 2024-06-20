// Copyright (C) 2024 Moritz Scheer

#ifndef _STATUS_H_
#define _STATUS_H_

#define MAX_TRIES       3
#define INVALID_ARG     -1000

/* -------------------- internal status codes -------------------- */
/* Error codes related to buffer */

#define NO_FREE_BUF     -1001

/* Error codes related to event handling */

#define RESPOND         -1002
#define DROP            -1003
#define RETRY           -1004

/* Error codes related to io_uring */

/* -------------------- external status codes -------------------- */

#define CQ_DONE         -1005
#define CQ_ERR          -1006
#define SQ_FULL         -1007
#define SQ_ERR          -1008

/* Error codes related to io_uring */

#define THREAD_ERR      -1009

/* Global error variable like errno */

#endif //_STATUS_H_
