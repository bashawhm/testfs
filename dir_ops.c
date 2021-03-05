#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <fuse_lowlevel.h>
#include <string.h>

#include "dir_ops.h"
#include "fs.h"

int internal_readdir(FileSystem *fs, const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    for (int i = 0; i < fs->num_files; i++) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        if (filler(buf, fs->files[i].file_name, &st, 0, FUSE_FILL_DIR_PLUS)) {
            break;
        }
    }

    return 0;
}

