// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <stdio.h>

void nonfatal_error(const char *syscall) {
	fprintf(stderr, "Async request failed: %s for event: %d\n", strerror(-cqe->res), req->event_type);
        exit(1);
}
