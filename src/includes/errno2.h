// Copyright (C) 2024 Moritz Scheer

#ifndef _ERRNO2_H_
#define _ERRNO2_H_

#define MAX_TRIES       3

#define QUEUE_FULL      -1000
#define EXIT            -1001
#define RESPOND         -1002
#define DROP            -1003
#define RETRY           -1004

extern int res;

#endif //_ERRNO2_H_
