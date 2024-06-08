// Copyright (C) 2024 Moritz Scheer

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void start_step(char *message)
{
	if (step == FIRST_STEP)
	{
		printf("-----------------------------------------------\n");
		printf("      Project configuration steps [0/%i]       \n", LAST_STEP);
		printf("-----------------------------------------------\n\n");
	}
	printf("[%i/%i] %s ...\n", step, LAST_STEP, message);
	fflush(stdout);
}

int end_step(char *message, int status_code)
{
	char *symbol = status_code == 0 ? SUCCESS_SYMBOL : ERROR_SYMBOL;
	printf("[%i/%i] %s %s\n", step, LAST_STEP, message, symbol);
	fflush(stdout);

	if (step == LAST_STEP)
	{
		printf("\n");
		printf("-----------------------------------------------\n");
		printf("             Project is configured             \n");
		printf("-----------------------------------------------\n");
		printf("\n");
	}
	step++;
	return status_code;
}

int print_status_code(int status_code)
{
	strerror(status_code);
	return status_code;
}