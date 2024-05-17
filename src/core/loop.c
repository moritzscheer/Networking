// Copyright (C) 2024 Moritz Scheer

#include <linux/types.h>
#include <linux/io_uring.h>
#include <liburing.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "loop.h"
#include "../handlers/read.h"
#include "../handlers/write.h"
#include "../includes/server.h"
#include "../includes/uring.h"
#include "../includes/io_buffer.h"
#include "../includes/errno2.h"

void server_loop(server *server, uring *uring)
{
	struct io_uring *ring;
	struct io_uring_cqe *cqe;
	int socket = server->socket;

	res = io_uring_queue_init(QUEUE_DEPTH, ring, 0);
	if (res != 0)
	{
		perror("Could not initialize io_uring queue.");
		return errno;
	}

	res = prepare_read(ring, socket);
	if (res != 0)
	{
		fprintf("failed to initialize io_uring queue.\n")
		io_uring_queue_exit(ring);
		return res;
	}

	/*
	 * Main Server Loop:
	 * This loop manages incoming HTTP connections, processes requests, writes responses if requested.
	 * It utilizes asynchronous I/O operations to handle multiple concurrent connections efficiently.
	 *
	 * Event Handling and Submission:
	 * The loop retrieves completed events from the Completion Queue Entry (CQE) structure, which stores information
	 * about completed asynchronous I/O operations. It then submits new asynchronous I/O operations to the server's
	 * event loop, enabling non-blocking handling of incoming requests.
	 */
	while (1)
	{
		int submissions = 0;

		res = io_uring_wait_cqe(ring, &cqe);
		if (res < 0)
		{
			perror("waiting for completion");
			break;
		}

		/*
		 * Request Processing Loop:
		 * This loop iterates through each received request, parsing and handling them sequentially.
		 *
		 * Asynchronous Operation Management:
		 * Additionally, the loop checks the status of the Submission Queue and pushes further asynchronous I/O
		 * operations to the Submission Queue Entry (CQE) structure for the next asynchronous operation.
		 */
		while (1)
		{
			io_buffer *io_buffer = (struct io_buffer *) cqe->user_data;

			switch (io_buffer->event_type)
			{
				case READ:
				{
					res = handle_read_result(server, io_buffer, cqe->res);

					switch (res)
					{
						case RESPOND:
							res = prepare_write(ring, socket, io_buffer);
							if(res == QUEUE_FULL)
								goto submit;
							submissions++;
							break;
						case RETRY:
							res = prepare_read(ring, socket, io_buffer);
							if(res == QUEUE_FULL)
								goto submit;
							else if (res != 0)
								return res;
							submissions++;
							break;
						case EXIT:
							return res;
					}

					res = prepare_read(ring, socket);
					if(res == QUEUE_FULL)
						goto submit;
					else if (res != 0)
						return res;
					submissions++;
					break;
				}
				case WRITE:
				{
					res = handle_write_result(server, io_buffer, cqe->res);

					if (res == RETRY && tries < MAX_TRIES)
					{
						res = prepare_write(ring, socket, io_buffer);
						if(res == QUEUE_FULL)
							goto submit;
						submissions++;
					}

					else free(io_buffer);
					break;
				}
				default:
				{
					perror("Unexpected request type");
					break;
				}
			}

			/* mark entry as seen */
			io_uring_cqe_seen(ring, cqe);

			/* the submission queue is full */
			if (io_uring_sq_space_left(ring) < MAX_SQE_PER_LOOP) break;

			/* no remaining work in completion queue */
			if (io_uring_peek_cqe(ring, &cqe) == -EAGAIN) break;
		}

		submit:

		/* submits the prepared io operations to the queue if any requests registered */
		if (submissions > 0)
			io_uring_submit(ring);
	}
	io_uring_queue_exit(ring);
	return 0;
}