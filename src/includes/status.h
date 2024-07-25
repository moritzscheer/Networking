// Copyright (C) 2024 Moritz Scheer

#ifndef _STATUS_H_
#define _STATUS_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

/* 1 : general error codes */

#define INVALID_ARG     1001

/* 2 : io_uring related error codes */

#define GET_SQE_ERR     2001

#define GET_MSG_OUT_ERR 2002

#define MSG_TRUNC_ERR   2003

/* 2 : thread related error codes */

#define THREAD_ERR      3001

/* 2 : ngtcp2 related error codes */

/* ------------------------------------------- ACTIONS DECLARATIONS ------------------------------------------------- */

#define NGTCP2_ERR_RETRY -1001

#define NGTCP2_ERR_STATELESS_RESET -1002

#define NGTCP2_ERR_DROP -1004

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

extern int res;

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_STATUS_H_
