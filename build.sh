gcc -pedantic testfs.c `pkg-config fuse3 --cflags --libs` -o testfs
