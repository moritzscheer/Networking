// Copyright (C) 2024 Moritz Scheer

#ifndef _READ_H_
#define _READ_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define READ 2

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */


/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */



/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int prepare_read(void);

int validate_read(struct io_uring_cqe *cqe);

static inline int resolve_success(void *iov_base, size_t iov_len);

static inline int resolve_error(int result_code);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_READ_H_