#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <fuse_lowlevel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "fs.h"
#include "file.h"

int internal_truncate(FileSystem *fs, const char *path, off_t size, struct fuse_file_info *fi) {
	fprintf(stderr, "TRUNCATING %s\n", path);

	if(strcmp(path, "/") != 0)
		return -ENOENT;

	return 0;
}

int internal_create(FileSystem *fs, const char *path, mode_t mode, struct fuse_file_info *fi) {
	fprintf(stderr, "CREATING %s\n", path);
    fs->num_files++;

    File *file = (File*)malloc(sizeof(File));
    file->file_name = strdup(path);
    file->file_buffer = NULL;
    file->buff_size = 0;

    fs->files = (File*)realloc(fs->files, (fs->num_files)*sizeof(File));
    fs->files[fs->num_files-1] = *file;

    return 0;
}

int internal_open(FileSystem *fs, const char *path, struct fuse_file_info *fi) {
	fprintf(stderr, "OPENING %s\n", path);

    if (strcmp(path, "/") == 0) {
        return 0;
    }


    if (fi->flags & O_CREAT) {
        fprintf(stderr, "REQUESTING FILE CREAT\n");
    }

    for (int i = 0; i < fs->num_files; i++) {
        if (strncmp(fs->files[i].file_name, path, strlen(fs->files[i].file_name)) == 0) {
            //return i+1;
            return 0;
        }
    }

//    fprintf(stderr, "REDIRECTING OPEN TO CREATE: %s\n", path);
//    internal_create(fs, path, 0, fi);

    return 0;
//    return -ENOENT;
}             

int internal_read(FileSystem *fs, const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	long int copy_len = size;
	long int copy_offset = offset;

	fprintf(stderr, "READING from %s\n", path);
    
	if (offset >= (1ULL << 32))
		return 0;

    for (int i = 0; i < fs->num_files; i++) {
        if (strncmp(fs->files[i].file_name, path, strlen(fs->files[i].file_name)) == 0) {
	        if (size > fs->files[i].buff_size) {
        		copy_len = fs->files[i].buff_size;
        	}
        	if (offset > fs->files[i].buff_size) {
        		return 0;
        	}
            if (copy_len == 0) {
                //fprintf(stderr, "attempting to read zero length file: %s\n", path);
                return 0;
            }
            memcpy(buf, fs->files[i].file_buffer + copy_offset, copy_len);
	        return copy_len;
        }
    }

	return -ENOENT;
}

int internal_write(FileSystem *fs, const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	fprintf(stderr, "WRITING to %s\n", path);

/*	file_buffer = (char*)realloc(file_buffer, size);
	buff_size = size;
	memcpy(file_buffer, buf, size);
*/
	if(strcmp(path, "/") != 0)
		return -ENOENT;

	return size;
}

int internal_mknod(FileSystem *fs, const char *path, mode_t mode, dev_t rdev) {
	fprintf(stderr, "MKNOD %s\n", path);
    

    return 0;
}

