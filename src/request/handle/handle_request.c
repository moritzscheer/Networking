// Copyright (C) 2024 Moritz Scheer

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../request.h"

int handle_request(Request *client_request)
{
	char http_request[1024];
	char *method, *path, *saveptr;

	if(get_line(req->iov[0].iov_base, http_request, sizeof(http_request))) 
	{
		fprintf(stderr, "Malformed request\n");
		exit(1);
	}
	
	method = strtok_r(method_buffer, " ", &saveptr);
	strtolower(method);
	path = strtok_r(NULL, " ", &saveptr);
	
	if (strcmp(method, "get") == 0) {
		handle_get_method(path, client_socket);
	}
	else {
		handle_unimplemented_method(client_socket);
	}
	
	return 0;
}
