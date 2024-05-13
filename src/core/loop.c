// Copyright (C) 2024 Moritz Scheer

#include <linux/io_uring.h>
#include <liburing.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "loop.h"
#include "../utils/io_operations.h"
#include "../handlers/io_handler.h"
#include "../includes/server.h"
#include "../includes/IORequest.h"

void server_loop(o_uring *ring, io_uring_cqe *cqe, int server_socket)
{
	while (MAIN_SERVER_LOOP)
	{
		int submissions = 0;

		status_code = io_uring_wait_cqe(ring, &cqe);
		if (status_code < 0)
		{
			perror("waiting for completion");
			break;
		}
5
		while (REQUESTS_IN_QUEUE)
		{
			IORequest *request = (IORequest *) cqe->user_data;

			switch (request->event_type)
			{
				case READ:
					if (cqe->res > 0)
					{
						/* ---- SUCCESSFUL ---- */
						parse_request(request);
						prep_write_io(ring, request);
						prep_read_io(ring, server_socket);
						*submissions += 2;
					}
					else if (cqe->res < 0)
					{
						/* ----- FAILURE ------ */
						// TODO: Handling of retries
					}
					else
					{
						/* TODO: Handling of stream */
						prep_read_io(ring, server_socket);
						*submissions += 1;
					}
					break;
				case WRITE:
					if (cqe->res > 0)
					{
						/* ---- SUCCESSFUL ---- */
						free(request);
					}
					else if (cqe->res == -1)
					{
						/* ----- FAILURE ------ */
						/* TODO: Handling of retries */
					}
					break;
				case CLOSE: free(request);
					break;
				default: fprintf(stderr, "Unexpected request type %d\n", request->event_type);
					break;
			}

			/* mark entry as seen */
			io_uring_cqe_seen(ring, cqe);

			/* the submission queue is full */
			if (io_uring_sq_space_left(ring) < MAX_SQE_PER_LOOP) break;

			/* no remaining work in completion queue */
			if (io_uring_peek_cqe(ring, &cqe) == -EAGAIN) break;
		}

		/* submits the prepared io operations to the queue if any requests registered */
		if (submissions > 0)
			io_uring_submit(ring);
	}

	io_uring_queue_exit(ring);
	return 0;
}