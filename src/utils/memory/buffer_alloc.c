// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <stdio.h>

void *buffer_alloc(size_t size)
{
	void *buffer = malloc(size);
	if(!buffer)
	{
		perror(stderr, "Fatal error: unable to allocate memory!\n");
		exit(1);
	}
	return buffer;
}
