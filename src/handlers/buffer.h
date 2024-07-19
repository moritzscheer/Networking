// Copyright (C) 2024, Moritz Scheer

#ifndef _BUFFER_H_
#define _BUFFER_H_

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

extern struct io_uring_buf_ring *buf_ring;

unsigned char *buffer_base;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int setup_buffer_pool();

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_BUFFER_H_
