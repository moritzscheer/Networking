#ifndef SERVER
#define SERVER

#define DOMAIN AF_INET6
#define PORT 8989
#define BUFSIZE 4096
#define BACKLOG 1
#define TYPE SOCK_STREAM
#define PROTOCOL 0

void handle_connection(int client_socket);

#endif
