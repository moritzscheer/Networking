// Copyright (C) 2024 Moritz Scheer

#ifndef _QUEUE_H_
#define _QUEUE_H_

struct queue
{
	struct pkt *head;
	struct pkt *tail;
};

struct pkt
{
	struct iovec *data;
	struct connection *connection;
	struct pkt *next;
};

struct queue_conn
{
	struct pkt_conn *head;
	struct pkt_conn *tail;
};

struct pkt_conn
{
	struct iovec *data;
	struct connection *connection;
	struct pkt_conn *next;
};

#endif //_QUEUE_H_
