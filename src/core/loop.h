// Copyright (C) 2024, Moritz Scheer

#ifndef _LOOP_H_
#define _LOOP_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define MAX_TASKS 100

#define BUFFER_LEN 4096

#define QD 64

#define BUF_SHIFT 12 /* 4k */

#define CQES (QD * 16)

#define BUFFERS CQES

#define CONTROLLEN 0

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

int listen_loop();

static int setup_ring();

static int cleanup_ring(int result);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_LOOP_H_
