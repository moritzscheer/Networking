//
// Created by Moritz on 8/2/24.
//

int handle_version_negotiation_pkt(uint8_t *pkt, size_t pktlen, struct sockaddr_storage *addr)
{
	size_t len = LONG_HEADER_PKT_MIN_LENGTH;

	size_t dcidlen = pkt[5];
	len += dcidlen;
	if (pktlen < len)
	{
		return 0;
	}
	uint8_t dcid = &pkt[6];

	size_t scidlen = pkt[6 + dcidlen];
	len += scidlen;
	if (pktlen < len)
	{
		return 0;
	}
	uint8_t scid = &pkt[6 + dcidlen + 1];

	uint8_t *buf = calloc(1, BUF_SIZE);
	if (!buf)
	{
		return ENOMEM;
	}

	uint8_t random;
	res = get_random(&random, sizeof(uint8_t));
	if (res != 0)
	{
		return res;
	}

	ssize_t len = ngtcp2_pkt_write_version_negotiation(buf, BUF_SIZE, random, dcid, dcidlen, scid, sclidlen,
	                                                   supported_versions, NUM_SUPPORTED_VERSIONS);
	uint8_t *resized_buf = realloc(buf, len);
	if (!resized_buf)
	{
		free(buf);
		return ENOMEM;
	}
	return prepare_write(resized_buf, len);
}