#pragma once

#include "fs.h"

int internal_truncate(FileSystem *fs, const char *path, off_t size, struct fuse_file_info *fi);
int internal_open(FileSystem *fs, const char *path, struct fuse_file_info *fi);
int internal_read(FileSystem *fs, const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int internal_write(FileSystem *fs, const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int internal_mknod(FileSystem *fs, const char *path, mode_t mode, dev_t rdev);
int internal_create(FileSystem *fs, const char *path, mode_t mode, struct fuse_file_info *fi);
