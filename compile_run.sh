#! /bin/bash

rm main

gcc -g -o main -I src/ src/main.c src/ramdisk.c \
      src/mxfs/bitmap.c src/mxfs/mxfs.c src/mxfs/inode_bitmap.c src/mxfs/block_bitmap.c

./main
