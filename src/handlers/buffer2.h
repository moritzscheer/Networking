// Copyright (C) 2024, Moritz Scheer

#ifndef _BUFFER_H_
#define _BUFFER_H_

#define INITIAL_SIZE 50

#define READ 1
#define WRITE 2
#define CLOSE 3

#define BUFFER_SIZE sizeof(struct msghdr) + (sizeof(int) * 2)
#define CONTROL_SIZE (CMSG_SPACE(sizeof(ExtraData)))
#define NUM_IOVEC 2

typedef struct {
	uint8_t event_type;
	uint8_t tries;
} Extra_Data;

typedef struct {
	struct msghdr *data;
	Buffer *next;
} Buffer;

int create_buffer_pool();
Buffer *get_buffer()
void free_buffer(Buffer *buffer);
void destroy_buffer_pool();

static Buffer *allocate_buffer();
static void free_buffer(Buffer *buffer);
static void reset_buffer(Buffer *buffer);

#endif //_BUFFER_H_
