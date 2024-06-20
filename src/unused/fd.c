// Copyright (C) 2024, Moritz Scheer

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

int check_fd(int fd)
{
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
	{
		perror("invalid file descriptor");
		return -1;
	}
	return 0;
}
