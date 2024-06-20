// Copyright (C) 2024, Moritz Scheer

#include <ngtcp2/ngtcp2.h>

int get_response_packet()
{
	return RESPOND;
}

int get_version_negotiation_packet()
{
	while(1)
	{
		if(ngtcp2_pkt_write_version_negotiation() == NGTCP2_ERR_NOBUF)
		{
			// todo: make buffer larger
			continue;
		}
		return 0;
	}
}

int get_retry_packet()
{
	while(1)
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
	while(1)
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
	while(1)
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

int send_status_response(unsigned int status_code)
{
	status_resp_body = make_status_body(status_code);

	respbuf.begin = respbuf.pos = reinterpret_cast<uint8_t * >(status_resp_body.data());
	respbuf.end = respbuf.last = respbuf.begin + status_resp_body.size();

	handler->add_sendq(this);
	handler->shutdown_read(stream_id, 0);

	return 0;
}

char *make_status_body(unsigned int status_code)
{
	auto status_string = util::format_uint(status_code);
	auto reason_phrase = http::get_reason_phrase(status_code);

	std::string body;
	body = "<html><head><title>";
	body += status_string;
	body += ' ';
	body += reason_phrase;
	body += "</title></head><body><h1>";
	body += status_string;
	body += ' ';
	body += reason_phrase;
	body += "</h1><hr><address>";
	body += NGTCP2_SERVER;
	body += " at port ";
	body += util::format_uint(config.port);
	body += "</address>";
	body += "</body></html>";
	return body;
}