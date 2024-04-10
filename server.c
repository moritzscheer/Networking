#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "server.h"
#include <sys/epoll.h>

// constant variables
#define DOMAIN AF_INET6
#define PORT 8989
#define BUFSIZE 4096
#define BACKLOG 1
#define TYPE SOCK_STREAM
#define PROTOCOL 0

// function prototypes
void handle_connection(int client_socket);

/*
 * setup_server() creates a Server object and sets up parameters and Sockets for receiving Messages.
 *
 * param: void
 * returns: struct Server
 *
 * author: Moritz Scheer
 * date: 09.04.2024
 */
struct Server setup_server(void)
{
	struct Server server = malloc(sizeof(struct Server));

	server.domain = DOMAIN;
	server.port = PORT;
	server.bufsize = BUFSIZE;
	server.backlog = BACKLOG;
	server.type = TYPE;
	server.protocol = PROTOCOL;
	server.address.sin_family = DOMAIN;
	server.address.sin_addr.s_addr = TYPE;
	server.address.sin_port = htons(PORT);

	if(server.socket = socket(DOMAIN, TYPE, PROTOCOL) < 0)
	{
		perror("Socket creation failed\n");
		exit(1);
	}
	printf("Socket creation successful\n");

	if(bind(server.socket, (SA*) &server.address, sizeof(server.address)) < 0)
	{
		perror("Socket binding failed\n");
		exit(1);
	}
	printf("Socket binding successful\n");
	
	if(listen(server_socket, backlog)) < 0)
	{
		perror("Socket listening failed\n");
		exit(1);
	}
	printf("Socket listening successful\n");

	return server;
}

/*
 * handle_connection() listens for incoming connections and handles further operations
 *
 * param: int server_socket
 * returns: void
 *
 * author: Moritz Scheer
 * date: 09.04.2024
 */
void handle_connection(int *server_socket)
{
	int addr_size = sizeof(struct sockaddr_in);
	int server_socket = &server->socket;

	while(1)
	{
		if(client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size) < 0)
		{
			perror("Socket accepting failed\n");
			exit(1);
		}

		valread = read(client_socket, server.buffer, 1024 - 1);
	}

	close(client_socket);
	return;
}
