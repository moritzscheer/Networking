

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "alloc_buffer.c"
#include "../error/fatal_error.c"

void copy_file_contents(char *file_path, off_t file_size, struct iovec *iov) 
{
    char *buf = buffer_alloc(file_size);

    int fd = open(file_path, O_RDONLY);
    if (fd < 0)
        fatal_error("read");

    int ret = read(fd, buf, file_size);
    if (ret < file_size)
        fprintf(stderr, "Encountered a short read.\n");

    close(fd);
    iov->iov_base = buf;
    iov->iov_len = file_size;
}
