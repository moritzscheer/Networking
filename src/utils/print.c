// Copyright (C) 2024 Moritz Scheer

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdarg.h>

void print_step(char *init_msg, char *err_msg, char *suc_msg, Function function, ...)
{
	if (step == FIRST_STEP)
	{
		printf("-----------------------------------------------\n");
		printf("      Project configuration steps [0/%i]       \n", LAST_STEP);
		printf("-----------------------------------------------\n\n");
	}
	printf("[%i/%i] %s ...\n", step, LAST_STEP, message);
	fflush(stdout);

	va_list args;
	va_start(args, function);
	res = function(args);
	va_end(args);

	char *symbol = res == 0 ? SUCCESS_SYMBOL : ERROR_SYMBOL;
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
}

int print_status_code(int status_code)
{
	switch (status_code)
	{
		case INVALID_ARG:
			break;
		case CQ_DONE:
			break;
		case CQ_ERR:
			break;
		case SQ_FULL:
			break;
		case GET_SQE_ERR:
			break;
		case GET_MSG_OUT_ERR:
			break;
		case MSG_TRUNC_ERR:
			break;
		case THREAD_ERR:
			break;
		default:
			strerror(status_code);
			break;
	}
	return status_code;
}