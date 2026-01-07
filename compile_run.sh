#! /bin/bash

gcc -g -o main -I src/ src/main.c src/ramdisk.c \
      src/mxfs/bitmap.c src/mxfs/mxfs.c 

./main
