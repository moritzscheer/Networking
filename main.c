#include "server.h"
#include "server.c"

int main()
{
    // create Server
    struct Server *server = setup_server();

    // open connections
    start_connection(&server);


}