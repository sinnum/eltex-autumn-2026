#ifndef FCOPY_H
#define FCOPY_H

#include <stddef.h>

struct file_header {
    char filename[256];
    long long size; // > 0 - size, 0 - not found, -1 - shutdown
};


void start_file_copy(char *filenames[], int file_count, const char *pipe_name);

#endif // FCOPY_H
