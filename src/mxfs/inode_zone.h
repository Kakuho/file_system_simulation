#ifndef MXFS_INODE_ZONE_H
#define MXFS_INODE_ZONE_H

#include <assert.h>

#include "types.h"
#include "mxfs.h"
#include "ramdisk.h"

RSTATUS mxfs_inode_zone_init(mxfs* mxfs, ramdisk* disk);
RSTATUS mxfs_inode_zone_setup_memory(mxfs* mxfs, ramdisk* disk);

RSTATUS mxfs_inode_zone_poison(mxfs* mxfs, ramdisk* disk, char ch);

size_t mxfs_inode_zone_nblocks(mxfs* mxfs);

void mxfs_inode_zone_index_to_block_offset(mxfs* mxfs, uint64_t inode_index, uint32_t* block_index, uint8_t* offset);

int64_t mxfs_inode_zone_set_inode(mxfs* mxfs, ramdisk* disk, uint64_t index, mx_disk_inode* inode);
int64_t mxfs_inode_zone_clear_inode(mxfs* mxfs, ramdisk* disk, uint64_t index);

#endif
