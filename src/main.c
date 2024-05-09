#include "server/core/init_socket.c"
#include "server/core/start_server.c"

int main()
{
    initialize_resources();

    configure_logging();

    start_server();
}
