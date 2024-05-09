// Copyright (C) 2024 Moritz Scheer
#ifndef SERVER
#define SERVER

typedef struct Response {
    char *headers;
    char *body;
    size_t body_length;
} Response;

#endif


