// Copyright (C) 2024 Moritz Scheer

#ifndef _RESOURCES_H_
#define _RESOURCES_H_

static int status_code;

int init_resources(Server *server);
void cleanup_resources(Server *server);

#endif //_RESOURCES_H_
