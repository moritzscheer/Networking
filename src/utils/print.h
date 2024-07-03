// Copyright (C) 2024 Moritz Scheer

#ifndef _PRINT_H_
#define _PRINT_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define FIRST_STEP              1

#define LAST_STEP               4

#define SUCCESS_SYMBOL          "\u2714"

#define FAILURE_SYMBOL          "\u2718"

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */



/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

static int step = FIRST_STEP;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

static void start_step(char *message);

static void end_step(char *message, int status_code);

void print_status_code(int status_code);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_PRINT_H_
