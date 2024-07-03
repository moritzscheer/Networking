// Copyright (C) 2024 Moritz Scheer

#include <string.h>
#include <stdlib.h>

struct pkt *create_packet(struct io_uring_recvmsg_out *msg_out, struct pkt *prev_pkt, ngtcp2_version_cid vcid)
{
	struct pkt *packet = calloc(1, sizeof(struct pkt));
	if (!packet)
	{
		return NULL;
	}

	ngtcp2_cid dcid;
	memset(&dcid, 0, sizeof(ngtcp2_cid));
	ngtcp2_cid_init(&dcid, vcid->dcid, vcid->dcidlen);

	ngtcp2_cid scid;
	memset(&scid, 0, sizeof(ngtcp2_cid));
	ngtcp2_cid_init(&scid, vcid->scid, vcid->scidlen);

	*packet = (struct pkt)
		{
			.dcid = dcid,
			.scid = scid,
			.receive_time = time(NULL),
			.iov_base = io_uring_recvmsg_payload(msg_out, msg),
			.iov_len = io_uring_recvmsg_payload_length(msg_out, BUFFER_LEN, msg),
			.prev_recv_pkt = prev_pkt
		};

	return packet;
}

void enqueue_packet()
{
	pthread_mutex_lock(&mutex);

	if (IsEmpty(available_queue))
	{
		int res = send_signal(&cond);
		if (res != 0)
		{
			pthread_mutex_unlock(&mutex);
			return res;
		}
	}

	if (IsEmpty(connection->packets))
	{
		Enqueue(available_queue, connection);
	}

	Enqueue(connection->packets, packet);

	pthread_mutex_unlock(&mutex);
}

void dequeue_packet()
{
	pthread_mutex_lock(&mutex);

	while (IsEmpty(available_queue))
	{
		res = pthread_cond_wait(&cond, &mutex);
		if (res != 0)
		{
			pthread_mutex_unlock(&mutex);
			return res;
		}
	}

	Dequeue(new_conn_queue, packet);

	pthread_mutex_unlock(&mutex);
}