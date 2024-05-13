#ifndef SERVER_CONF
#define SERVER_CONF

// ------------------------------------------------------------------
// server config makros
// ------------------------------------------------------------------

#define DOMAIN AF_INET

#define IPPROTO_UDP 17

#define PORT 8989

#define BACKLOG 1

#define BUFSIZE 4096

#define CONN_TIMEOUT 30
#define READ_TIMEOUT 10
#define WRITE_TIMEOUT 10

#define MAX_CONNECTIONS 100

// ------------------------------------------------------------------

#endif
