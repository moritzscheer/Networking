// Copyright (C) 2024 Moritz Scheer

#ifndef _MEMORY_H_
#define _MEMORY_H_

void *allocate_memory(size_t num, size_t size);
int allocate_base_memory(Server *server);

#endif //_MEMORY_H_
