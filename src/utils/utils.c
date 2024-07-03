// Copyright (C) 2024 Moritz Scheer

#include <time.h>
#include <sys/random.h>

ngtcp2_tstamp timestamp_ns()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ngtcp2_tstamp) ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

ssize_t get_random(void *data, size_t datalen)
{

	ssize_t random = getrandom(data, datalen);
	if (random < 0)
	{
		usleep(1000);
		return getrandom(data, datalen);
	}
	return random;
}

