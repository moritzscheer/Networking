// Copyright (C) 2024 Moritz Scheer

int configure_ngtcp2(ngtcp2_settings *settings)
{
	ngtcp2_settings_default(settings);
	server.settings.initial_ts = timestamp();
	server.settings.log_printf = log_printf;
}