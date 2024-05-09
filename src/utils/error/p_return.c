// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <stdio.h>

void p_return(const char *syscall) {
	return fprintf(stderr, "Async request failed: %s for event: %d\n", strerror(-cqe->res), req->event_type);
}
