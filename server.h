#ifndef SERVER
#define SERVER

#include <stdlib.h>

struct Server 
{
	int domain;
	int port;
	int bufsize;
	int backlog;
	int type;
	int protocol;
	struct sockaddr_in address;
	int socket;
	int epollfd;
}

#endif
