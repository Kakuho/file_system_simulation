#ifndef MXFS_MXFS_H
#define MXFS_MXFS_H

#include "ramdisk.h"
#include "bitmap.h"
#include "types.h"
#include "path_indexer.h"

typedef struct mxfs{
  mx_bitmap inode_bitmap;
  mx_bitmap block_bitmap; 
  mx_superblock superblock;
} mxfs;

void mxfs_init(mxfs* mxfs, ramdisk* disk, size_t ninodes, size_t nblocks);
// initially root directory should look like:
//    /
//      a.txt       1, a.txt
//      b.txt       2, b.txt
//      c.txt       3, c.txt
RSTATUS mxfs_init_root_directory(mxfs* mxfs, ramdisk* disk);

RSTATUS mxfs_flush_superblock(mxfs* mxfs, ramdisk* disk);
RSTATUS mxfs_refresh_superblock(mxfs* mxfs, ramdisk* disk);

RSTATUS mxfs_write_superblock(ramdisk* disk, char* buffer);
RSTATUS mxfs_read_superblock(ramdisk* disk, char* buffer);

RSTATUS mxfs_get_inode(mxfs* mxfs, ramdisk* disk, uint64_t index, mx_disk_inode* buffer);
RSTATUS mxfs_set_inode(mxfs* mxfs, ramdisk* disk, uint64_t index, mx_disk_inode* inode);
RSTATUS mxfs_clear_inode(mxfs* mxfs, ramdisk* disk, uint64_t index);

RSTATUS mxfs_set_block(mxfs* mxfs, ramdisk* disk, uint64_t index, char* buffer);
RSTATUS mxfs_clear_block(mxfs* mxfs, ramdisk* disk, uint64_t index);

int32_t mxfs_path_to_inode(mxfs* mxfs, ramdisk* disk, const char* path);

#endif
