#pragma once

#include "file.h"

typedef struct {
    File *files;
    unsigned int num_files;
    char *mount_point;
} FileSystem;
