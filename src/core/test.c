//
// Created by morit on 17.06.2024.
//

int listen_loop(struct server *server)
{
	int res, I, count;

	res = setup_context(ring, cqes, msg);
	if (res != 0)
	{
		return res;
	}

	res = prepare_read(ring, server->socket);
	if (res != 0)
	{
		return cleanup(res);
	}

	while (1)
	{
		res = io_uring_submit_and_wait(ring, 1);
		if (res == -EINTR)
		{ /* interrupted syscall */
			continue;
		}
		else if (res < 0)
		{
			return cleanup(res);
		}

		/*
		 * Gets the amount of Completion events from the ring and loops over them.
		 */
		count = io_uring_peek_batch_cqe(ring, &cqes[0], CQES);
		for (I = 0; I < count; I++)
		{
			res = precess_cqe(cqe);
			if (res != 0)
			{
				return cleanup(res);
			}
		}
		io_uring_cq_advance(ring, count);
	}
}