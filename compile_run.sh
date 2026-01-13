#! /bin/bash

rm main

gcc -g -o main -I src/ src/main.c src/ramdisk.c src/path_indexer.c\
      src/debug/debug_log.c \
      src/mxfs/bitmap.c src/mxfs/mxfs.c src/mxfs/inode_bitmap.c src/mxfs/block_bitmap.c \
      src/mxfs/inode_zone.c src/mxfs/block_zone.c


./main
