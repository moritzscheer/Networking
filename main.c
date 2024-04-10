#include "server.h"
#include "server.c"

int main()
{
	struct Server server = setup_server();
	handle_connections(server);
}

