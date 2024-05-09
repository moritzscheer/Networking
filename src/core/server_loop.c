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
#include "../handlers/io_handler.c"
#include "../../includes/io_entry.h"
#include "../../includes/connection.h"
#include "../../lib/uthash/uthash.h"

void server_loop(int server_socket)
{
    struct io_uring ring;
    struct io_uring_cqe *cqe;
    struct connection *connections = NULL;

    if(io_uring_queue_init(QUEUE_DEPTH, &ring, 0) < 0)
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
            io_entry *entry = (io_entry *) cqe->user_data;

            switch (entry->event_type)
            {
                case READ:
                    if(cqe->res > 0)
                    {
                        response = handle_http_request();
                        add_write_request(&ring, response);
                        add_read_request(&ring, entry->client_socket);
                        submissions += 2;
                    }
                    else if(cqe->res < 0)
                    {
                        strerror(-cqe->res);
                        add_close_request(&ring, request);
                    }
                    else
                    {
                        fprintf(stderr, "Zero-length read: no data or end of stream.\n");
                        add_read_request(ring, request->client_socket);
                        submissions += 1;

                    }
                    break;
                case WRITE:
                    if(cqe->res > 0)
                    {
                        free_response(response);
                        free_request(request);
                    }
                    else if(cqe->res < 0)
                    {

                    }
                    else
                    {
                        add_write_request(&ring, response);
                    }
                    break;
                case CLOSE:
                    free_response(response);
                    free_request(request);
                    break;
                default:
                    fprintf(stderr, "Unexpected request type %d\n", request->type);
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
