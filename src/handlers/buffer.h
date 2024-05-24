// Copyright (C) 2024, Moritz Scheer

#ifndef _BUFFER_H_
#define _BUFFER_H_

#define INITIAL_SIZE            50

#define READ                    1
#define WRITE                   2

/* ----------------------------------------- */
/* Macro functions with predefined constants */
/* ----------------------------------------- */

#define CMSG_NUM                1
#define IOV_NUM                 2
#define IOV_LEN                 69931

#define MSGHDR_LEN              (sizeof(struct msghdr))
#define MSGHDR_START(start)     ((struct msghdr *) start)

#define CMSGHDR_LEN             (CMSG_NUM * CMSG_SPACE(sizeof(Extra_Data)))
#define CMSGHDR_START(start)    ((struct cmsghdr *) (start + MSGHDR_LEN))

#define IOVEC_LEN               (IOV_NUM * sizeof(struct iovec))
#define IOVEC_START(start)      ((struct iovec *) (start + MSGHDR_LEN + CMSGHDR_LEN))

#define IOV_BASE_LEN            (IOV_NUM * IOV_LEN)
#define IOV_BASE_START(start)   ((void *) (start + MSGHDR_LEN + CMSGHDR_LEN + IOVEC_LEN))

/* ----------------------------------------------- */
/* Macro functions with variable number of structs */
/* ----------------------------------------------- */

#define CMSGHDR_LEN_NUM(num)                (num * CMSG_SPACE(sizeof(Extra_Data)))

#define IOVEC_LEN_NUM(num)                  (num * sizeof(struct iovec))
#define IOVEC_START_NUM(start, cmsg_num)    ((struct iovec *) (start + MSGHDR_LEN + CMSGHDR_LEN_NUM(cmsg_num)))

#define GET_EVENT_TYPE(start)               ((uint8_t *) CMSG_DATA(CMSG_FIRSTHDR(((void *) start))))

#define GET_TRIES(start)                    ((uint8_t *) CMSG_DATA(CMSG_FIRSTHDR(((void *) start))) + sizeof(uint8_t))

typedef struct {
	uint8_t event_type;
	uint8_t tries;
} Extra_Data;

extern int create_buffer_pool(void);

extern struct msghdr *get_buffer(void);

extern void return_buffer(struct msghdr *msghdr);

extern void destroy_buffer_pool(void);

static struct msghdr *allocate_buffer(void);

static void reset_buffer(struct msghdr *msghdr);

#endif //_BUFFER_H_
