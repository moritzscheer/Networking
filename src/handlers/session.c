// Copyright (C) 2024 Moritz Scheer

int configure_session()
{
	start_step("Configure session");

	int res = 0;
	if (res != 0)
	{
		return end_step("Failed to configure session", res);
	}

	return end_step("Session configured", 0);
}