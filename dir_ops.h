#pragma once

#include "fs.h"

int internal_readdir(FileSystem *fs, const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags);
