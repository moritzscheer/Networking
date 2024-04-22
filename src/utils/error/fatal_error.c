// Copyright (C) 2024 Moritz Scheer

#include <stdlib.h>
#include <stdio.h>

void fatal_error(const char *syscall) {
    perror(syscall);
    exit(1);
}
