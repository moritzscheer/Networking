// Copyright (C) 2024 Moritz Scheer

#include <nghttp3/nghttp3.h>
#include <string.h>
#include "../handlers/connection.h"

int setup_nghttp3(void)
{
	/*
	 * sets up default default_settings struct with custom initial value
	 */
	nghttp3_conn_settings_default(&default_settings);

	return 0;
}

int create_ngtcp2_conn(struct connection *connection)
{
	nghttp3_settings settings;
	memcpy(&settings, &default_settings, sizeof(ngtcp2_settings));
	settings.max_field_section_size = NULL;
	settings.qpack_max_dtable_capacity = NULL;
	settings.qpack_encoder_max_dtable_capacity = NULL;
	settings.qpack_blocked_streams = NULL;
	settings.h3_datagram = NULL
	settings.enable_connect_protocol = NULL;

	nghttp3 *conn = NULL;
	if (nghttp3_conn_server_new(&conn, &callbacks, &settings, NULL) != 0)
	{
		return NOMEM;
	}

	return 0;
}
