#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <fuse_lowlevel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include "fs.h"
#include "file_ops.h"
#include "dir_ops.h"

FileSystem fs;

void init_fs(char *mount_point) {
    fs.mount_point = strdup(mount_point);
    fs.num_files = 0;
    fs.files = NULL;
}

static int tfs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
	fprintf(stderr, "GETATTR-ING\n");
    /*
	if(strcmp(path, "/") != 0)
		return -ENOENT;
    */

	stbuf->st_mode = S_IFREG | 0644;
	stbuf->st_nlink = 1;
	stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_size = (1ULL << 32); /* 4G */
	stbuf->st_blocks = 0;
	stbuf->st_atime = stbuf->st_mtime = stbuf->st_ctime = time(NULL);

	return 0;
}

int tfs_truncate(const char *path, off_t size, struct fuse_file_info *fi) {
    return internal_truncate(&fs, path, size, fi); 
}

int tfs_open(const char *path, struct fuse_file_info *fi) {
    return internal_open(&fs, path, fi);
}

int tfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    return internal_read(&fs, path, buf, size, offset, fi);
}

int tfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    return internal_write(&fs, path, buf, size, offset, fi);
}

int tfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    return internal_readdir(&fs, path, buf, filler, offset, fi, flags);
}

static const struct fuse_operations tfs_oper = {
	.getattr	= tfs_getattr,
	.truncate	= tfs_truncate,
	.open		= tfs_open,
	.read		= tfs_read,
	.write		= tfs_write,
    .readdir    = tfs_readdir,
};

int main(int argc, char *argv[])
{
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	struct fuse_cmdline_opts opts;
	struct stat stbuf;

	if (fuse_parse_cmdline(&args, &opts) != 0)
		return 1;
	fuse_opt_free_args(&args);

	if (!opts.mountpoint) {
		fprintf(stderr, "missing mountpoint parameter\n");
		return 1;
	}

	if (stat(opts.mountpoint, &stbuf) == -1) {
		fprintf(stderr ,"failed to access mountpoint %s: %s\n",
			opts.mountpoint, strerror(errno));
		free(opts.mountpoint);
		return 1;
	}

	if (!S_ISDIR(stbuf.st_mode)) {
		fprintf(stderr, "mountpoint is not a directory\n");
		return 1;
	}

	printf("Mounting at point: %s\n", opts.mountpoint);
    init_fs(opts.mountpoint);

	free(opts.mountpoint);

	return fuse_main(argc, argv, &tfs_oper, NULL);
}
