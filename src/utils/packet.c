// Copyright (C) 2024 Moritz Scheer

int get_version_negotiation_packet()
{
	while (1)
	{
		if (ngtcp2_pkt_write_version_negotiation() == NGTCP2_ERR_NOBUF)
		{
			// todo: make buffer larger
			continue;
		}
		return 0;
	}
}

int get_retry_packet()
{
	while (1)
	{
		switch (ngtcp2_crypto_write_retry())
		{
			case NGTCP2_ERR_NOBUF:
				// todo: make buffer larger
				continue;
			case NGTCP2_ERR_CALLBACK_FAILURE:
				// todo: callback failure
				return DROP;
			case NGTCP2_ERR_INVALID_ARGUMENT:
				// todo: invalid arg
				return DROP;
			default:
				return RESPOND;
		}
	}
}

int get_connection_close_packet()
{
	while (1)
	{
		switch (ngtcp2_pkt_write_connection_close())
		{
			case NGTCP2_ERR_NOBUF:
				// todo: make buffer larger
				continue;
			case NGTCP2_ERR_CALLBACK_FAILURE:
				// todo: callback failure
				return DROP;
			default:
				return RESPOND;
		}
	}
}

int get_stateless_reset_packet()
{
	while (1)
	{
		switch (ngtcp2_pkt_write_stateless_reset())
		{
			case NGTCP2_ERR_NOBUF:
				// todo: make buffer larger
				continue;
			case NGTCP2_ERR_CALLBACK_FAILURE:
				// todo: callback failure
				return DROP;
			default:
				return RESPOND;
		}
	}
}

int prepare_packet()
{
	return 0;
}