#pragma once

#include <dirent.h>

#include "fs.h"

typedef struct {
    DIR *dp;
    struct dirent *ent;
    off_t offset;
} TFSDir;

int internal_readdir(FileSystem *fs, const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags);
int internal_opendir(FileSystem *fs, const char *path, struct fuse_file_info *fi);
