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

int get_timestamp_ns2(ngtcp2_tstamp tstamp)
{
	struct timespec ts;

	int res = clock_gettime(CLOCK_MONOTONIC, &ts);
	if (res == 0)
	{
		tstamp = ts.tv_sec * (int64_t) NGTCP2_SECONDS + ts.tv_nsec
	}
	return res;
}

int get_random(void *data, size_t datalen)
{
	ssize_t random = getrandom(data, datalen);
	if (random < 0)
	{
		usleep(1000);
		return getrandom(data, datalen);
	}
	return random;
}