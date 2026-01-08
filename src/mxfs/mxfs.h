#ifndef MXFS_MXFS_H
#define MXFS_MXFS_H

#include "ramdisk.h"
#include "bitmap.h"
#include "types.h"

typedef struct mxfs{
  mx_bitmap inode_bitmap;
  mx_bitmap block_bitmap; 
  mx_superblock superblock;
} mxfs;

void mxfs_init(mxfs* mxfs, ramdisk* disk, size_t ninodes, size_t nblocks);

RSTATUS mxfs_flush_superblock(mxfs* mxfs, ramdisk* disk);
RSTATUS mxfs_refresh_superblock(mxfs* mxfs, ramdisk* disk);

RSTATUS mxfs_write_superblock(ramdisk* disk, char* buffer);
RSTATUS mxfs_read_superblock(ramdisk* disk, char* buffer);

#endif
