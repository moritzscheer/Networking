// Copyright (C) 2024 Moritz Scheer

#ifndef _PRINT_H_
#define _PRINT_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */

#define FIRST_STEP              1

#define LAST_STEP               4

#define SUCCESS_SYMBOL          "\u2714"

#define FAILURE_SYMBOL          "\u2718"

/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

typedef int (*Function)(va_list);

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */

static int step = FIRST_STEP;

/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

void print_step(char *init_msg, char *err_msg, char *suc_msg, Function function, ...);

void print_status_code(int status_code);

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_PRINT_H_
