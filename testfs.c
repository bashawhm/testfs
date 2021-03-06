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
	fprintf(stderr, "INIT FS\n");

    fs.mount_point = strdup(mount_point);
    fs.num_files = 0;
    fs.files = NULL;
}

static int tfs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
//	fprintf(stderr, "GETATTR-ING\n");

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0555;
	    stbuf->st_nlink = 2;
    } else {
	    stbuf->st_mode = S_IFREG | 0644;
    	stbuf->st_nlink = 1;
    }
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

int tfs_mknod(const char *path, mode_t mode, dev_t rdev) {
    return internal_mknod(&fs, path, mode, rdev);
}

int tfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    return internal_create(&fs, path, mode, fi);
}

int tfs_utimens(const char *path, const struct timespec ts[2], struct fuse_file_info *fi) {
    fprintf(stderr, "UTIMENS %s\n", path);
    return 0;
}

int tfs_mkdir(const char *path, mode_t mode) {
    fprintf(stderr, "MKDIR %s\n", path);
    return 0;
}

int tfs_rmdir(const char *path) {
    fprintf(stderr, "RMDIR %s\n", path);
    return 0;
}

int tfs_symlink(const char *from, const char *to) {
    fprintf(stderr, "SYMLINK from %s to %s\n", from, to);
    return 0;
}

int tfs_rename(const char *from, const char *to, unsigned int flags) {
    fprintf(stderr, "RENAME from %s to %s\n", from, to);
    return 0;
}

int tfs_link(const char *from, const char *to) {
    fprintf(stderr, "LINK from %s to %s\n", from, to);
    return 0;
}

int tfs_listxattr(const char *path, char *list, size_t size) {
    fprintf(stderr, "LISTXATTR %s\n", path);
    return 0;
}

int tfs_statfs(const char *path, struct statvfs *stbuf) {
    fprintf(stderr, "STATFS %s\n", path);
    return 0;
}

int tfs_opendir(const char *path, struct fuse_file_info *fi) {
    return internal_opendir(&fs, path, fi);
}

static const struct fuse_operations tfs_oper = {
	.getattr	= tfs_getattr,
	.truncate	= tfs_truncate,
	.open		= tfs_open,
	.read		= tfs_read,
	.write		= tfs_write,
    .mknod      = tfs_mknod,
    .create     = tfs_create,
    .rename     = tfs_rename,
    .utimens    = tfs_utimens,
    .symlink    = tfs_symlink,
    .link       = tfs_link,
    .opendir    = tfs_opendir,
    .readdir    = tfs_readdir,
    .mkdir      = tfs_mkdir,
    .rmdir      = tfs_rmdir,
    .listxattr  = tfs_listxattr,
    .statfs     = tfs_statfs,
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
