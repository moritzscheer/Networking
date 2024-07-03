// Copyright (C) 2024 Moritz Scheer

#include "session.h"
#include "../includes/server.h"

int initialize_session(struct server *server)
{
	int res = 0;

	if (res != 0)
	{
		return res;
	}

	return res;
}

int recv_crypto_data
	(ngtcp2_conn *conn,
	 ngtcp2_encryption_level encryption_level,
	 uint64_t offset,
	 const uint8_t *data,
	 size_t datalen,
	 void *user_data)
{
	if (!config.quiet && !config.no_quic_dump)
	{
		print_crypto_data(encryption_level, {data, datalen});
	}

	return ngtcp2_crypto_recv_crypto_data_cb(conn, encryption_level, offset, data, datalen, user_data);
}

void print_crypto_data(ngtcp2_encryption_level encryption_level, uint8_t data)
{
	const char *encryption_level_str;

	switch (encryption_level)
	{
		case NGTCP2_ENCRYPTION_LEVEL_INITIAL:
			encryption_level_str = "Initial";
			break;
		case NGTCP2_ENCRYPTION_LEVEL_HANDSHAKE:
			encryption_level_str = "Handshake";
			break;
		case NGTCP2_ENCRYPTION_LEVEL_1RTT:
			encryption_level_str = "1-RTT";
			break;
		default:
			assert(0);
	}
	printf("Ordered CRYPTO data in %s crypto level\n", encryption_level_str);
}