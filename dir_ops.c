#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <fuse_lowlevel.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "dir_ops.h"
#include "fs.h"

int internal_readdir(FileSystem *fs, const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    fprintf(stderr, "READDIR %s | offset = %d | numfiles = %d\n", path, offset, fs->num_files);

    TFSDir *d = fi->fh;

    for (int i = 0; i < fs->num_files; i++) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_mode = S_IFREG | 0644;

        //fprintf(stderr, "file name = |%s|\n", fs->files[i].file_name);
        if (filler(buf, fs->files[i].file_name, NULL, 0, 0)) {
            fprintf(stderr, "Error filling file %s\n", fs->files[i].file_name);
            break;
        }
    }

    return 0;
}

int internal_opendir(FileSystem *fs, const char *path, struct fuse_file_info *fi) {
    fprintf(stderr, "OPENDIR %s\n", path);

    TFSDir *d = (TFSDir *)malloc(sizeof(TFSDir));

    d->offset = 0;
    d->ent = NULL;

    fi->fh = (unsigned long) d;
    return 0;
}
