#include "./server/setup_socket/setup_socket.c"
#include "./server/server_loop/server_loop.c"

int main()
{
    initialize_resources();

    configure_logging();

    start_server();

    handle_shutdown();
}
