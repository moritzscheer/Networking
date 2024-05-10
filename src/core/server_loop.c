// Copyright (C) 2024 Moritz Scheer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <liburing.h>
#include "../handlers/io_handler.h"
#include "../handlers/parse_request.h"
#include "../../includes/io_entry.h"
#include "../../includes/connection.h"
#include "../../lib/uthash/uthash.h"

void server_loop(int server_socket)
{
	struct io_uring ring;
	struct io_uring_cqe *cqe;
	struct connection *connections = NULL;

	if (io_uring_queue_init(QUEUE_DEPTH, &ring, 0) < 0)
		p_exit("Could not initialize io_uring queue.");
	add_read_request(&ring, server_socket);

	while (1)
	{
		int submissions = 0;
		int ret = io_uring_wait_cqe(&ring, &cqe);
		if (ret < 0)
		{
			perror("Error waiting for completion");
			break;
		}

		while (1)
		{
			IORequest *io_request = (IORequest *) cqe->user_data;

			switch (io_request->event_type)
			{
				case READ:
					if (cqe->res > 0)
					{
						parse_request(io_request);
						prep_write_io(&ring, io_request);
						prep_read_io(&ring, server_socket);
						submissions += 2;
					}
					else if (cqe->res < 0)
					{
						strerror(-cqe->res);
						prep_close_io(&ring, io_request);
					}
					else
					{
						fprintf(stderr, "Zero-length read: no data or end of stream.\n");
						prep_read_io(ring, server_socket);
						submissions += 1;
					}
					break;
				case WRITE:
					if (cqe->res > 0)
					{
						free_request(io_request);
					}
					else if (cqe->res < 0)
					{

					}
					else
					{
						add_write_request(&ring, io_request);
					}
					break;
				case CLOSE:free_request(io_request);
					break;
				default:fprintf(stderr, "Unexpected io_request type %d\n", io_request->type);
					break;
			}
			io_uring_cqe_seen(&ring, cqe);

			if (io_uring_sq_space_left(&ring) < MAX_SQE_PER_LOOP)
			{
				break; // the submission queue is full
			}

			ret = io_uring_peek_cqe(&ring, &cqe);
			if (ret == -EAGAIN)
			{
				break; // no remaining work in completion queue
			}
		}

		if (submissions > 0)
		{
			io_uring_submit(&ring);
		}
	}
	io_uring_queue_exit(&ring);
}