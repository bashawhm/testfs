gcc -pedantic testfs.c file_ops.c dir_ops.c `pkg-config fuse3 --cflags --libs` -o testfs
