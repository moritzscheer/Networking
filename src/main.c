#include "server.h"
#include "server.c"

int main()
{
    // create Server
    int server_socket = setup_server();

    // open connections
    start_connection(server_socket);


}
