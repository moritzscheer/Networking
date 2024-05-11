// Copyright (C) 2024 Moritz Scheer

#include "server/core/init_socket.c"

int main()
{
	struct Server server;
	int result;

	result = init_resources(server);
	if (result < 0) p_exit("could not initialize resources");

	result = configure_logging(server);
	if (result < 0) p_exit("could not configure logging");

	result = server_loop(server);
	handle_shutdown(server_socket, threads);
	return result < 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
