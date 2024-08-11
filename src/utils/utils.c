// Copyright (C) 2024 Moritz Scheer

#include <time.h>
#include <sys/random.h>

struct address
{
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];
};

ngtcp2_tstamp get_timestamp_ns()
{
	struct timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
	{
		return 0;
	}

	return ts.tv_sec * (int64_t) NGTCP2_SECONDS + ts.tv_nsec;
}

int get_random(void *buf, size_t buflen)
{
	size_t bytes_read = 0;
	while (bytes_read < buflen)
	{
		ssize_t result = getrandom((uint8_t *) buf + bytes_read, buflen - bytes_read, 0);
		if (result == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				return -errno;
			}
		}
		bytes_read += result;
	}
	return (int) bytes_read;
}