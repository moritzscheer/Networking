// Copyright (C) 2024 Moritz Scheer

#include <string.h>
#include "./core/resources.h"
#include "./includes/server.h"
#include "./includes/status.h"

int main(void)
{
	memset(&server, 0, sizeof(struct server));

	res = initialize_resources();
	if (res == 0)
	{
		res = server_loop();
	}

	return handle_shutdown(res);
}