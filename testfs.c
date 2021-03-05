#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <fuse_lowlevel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

static int tfs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
	(void) fi;

	fprintf(stderr, "GETATTR-ING\n");

	if(strcmp(path, "/") != 0)
		return -ENOENT;

	stbuf->st_mode = S_IFREG | 0644;
	stbuf->st_nlink = 1;
	stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_size = (1ULL << 32); /* 4G */
	stbuf->st_blocks = 0;
	stbuf->st_atime = stbuf->st_mtime = stbuf->st_ctime = time(NULL);

	return 0;
}

static int tfs_truncate(const char *path, off_t size, struct fuse_file_info *fi) {
	(void) size;
	(void) fi;

	fprintf(stderr, "TRUNCATING\n");

	if(strcmp(path, "/") != 0)
		return -ENOENT;

	return 0;
}

static int tfs_open(const char *path, struct fuse_file_info *fi) {
	(void) fi;

	fprintf(stderr, "OPENING\n");

	if(strcmp(path, "/") != 0)
		return -ENOENT;

	return 0;
}             

char *file_buffer = NULL;
long int buff_size = 0;

static int tfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	(void) buf;
	(void) offset;
	(void) fi;

	long int copy_len = size;
	long int copy_offset = offset;

	fprintf(stderr, "READING\n");

	if (size > buff_size) {
		copy_len = buff_size;
	}
	if (offset > buff_size) {
		return 0;
	}

	memcpy(buf, file_buffer + offset, copy_len);

	if(strcmp(path, "/") != 0)
		return -ENOENT;

	if (offset >= (1ULL << 32))
		return 0;

	return size;
}

static int tfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	(void) buf;
	(void) offset;
	(void) fi;

	fprintf(stderr, "WRITING\n");

	file_buffer = (char*)realloc(file_buffer, size);
	buff_size = size;
	memcpy(file_buffer, buf, size);

	if(strcmp(path, "/") != 0)
		return -ENOENT;

	return size;
}

static const struct fuse_operations tfs_oper = {
	.getattr	= tfs_getattr,
	.truncate	= tfs_truncate,
	.open		= tfs_open,
	.read		= tfs_read,
	.write		= tfs_write,
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

	if (!S_ISREG(stbuf.st_mode)) {
		fprintf(stderr, "mountpoint is not a regular file\n");
		return 1;
	}

	printf("Mounting at point: %s\n", opts.mountpoint);
	free(opts.mountpoint);

	return fuse_main(argc, argv, &tfs_oper, NULL);
}
