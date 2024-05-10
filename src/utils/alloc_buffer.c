// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint8_t *alloc_buffer(size_t size)
{
    uint8_t *buffer = malloc(size);
    memset(&buffer, 0, size);
	if(!buffer)
	{
		perror(stderr, "Fatal error: unable to allocate memory!\n");
		exit(1);
	}
	return buffer;
}
