// Copyright (C) 2024 Moritz Scheer

#ifndef _RETRY_H_
#define _RETRY_H_

#include <stdarg.h>

#define MAX_TRIES 5

typedef int (*retry_func)(va_list args);

int retry(retry_func func, ...)
{
	va_list args;
	va_start(args, func);
	int res;
	uint8_t tries = 0;
	do
	{
		res = func(args);
		if (res == 0)
		{
			return 0;
		}
		tries++;
	}
	while (tries < MAX_TRIES);
	va_end(args);
	return res;
}

#endif //_RETRY_H_
