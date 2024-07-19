// Copyright (C) 2024 Moritz Scheer

#include <time.h>
#include <sys/random.h>

ngtcp2_tstamp timestamp_ns()
{
	struct timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
	{
		return 0;
	}

	return (uint64_t) ts.tv_sec * NGTCP2_SECONDS + (uint64_t) ts.tv_nsec;
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

int get_random_cid(ngtcp2_cid *cid, size_t len)
{
	if (len > NGTCP2_MAX_CIDLEN)
	{
		return -1;
	}

	cid->datalen = len;
	rand_bytes(cid->data, cid->datalen);

	return 0;
}

ngtcp2_cid *generate_cid()
{
	ngtcp2_cid *cid;
	uint8_t buf[NGTCP2_MAX_CIDLEN];

	if (RAND_bytes(buf, sizeof(buf)) != 1)
	{
		return NULL;
	}
	ngtcp2_cid_init(cid, buf, sizeof(buf));
	return cid;
}