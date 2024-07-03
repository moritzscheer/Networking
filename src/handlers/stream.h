// Copyright (C) 2024, Moritz Scheer

#ifndef _STREAM_H_
#define _STREAM_H_

/* -------------------------------------------- MACRO DECLARATIONS -------------------------------------------------- */



/* ------------------------------------------- STRUCT DECLARATIONS -------------------------------------------------- */

struct Stream
{
	int64_t id;
	Queue *data;
	bool closed;
	/* invariant: sent_offset >= acked_offset */
	size_t sent_offset;
	size_t acked_offset;
};

/* --------------------------------------- GLOBAL VARIABLES DECLARATIONS -------------------------------------------- */



/* ------------------------------------------- FUNCTION DECLARATIONS ------------------------------------------------ */

Stream *create_stream(int64_t id);

void close_stream();

int stream_push_data();

void stream_finish_data();

/* ------------------------------------------------------------------------------------------------------------------ */

#endif //_STREAM_H_