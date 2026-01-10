#ifndef INODE_ZONE_H
#define INODE_ZONE_H

#include "types.h"
#include "mxfs.h"
#include "ramdisk.h"

RSTATUS mxfs_inode_zone_init(mxfs* mxfs, ramdisk* disk, uint16_t base, uint64_t ninodes);
RSTATUS mxfs_inode_zone_setup_memory(ramdisk* disk, uint16_t base, uint64_t ninodes);

size_t mxfs_inode_zone_nblocks(mxfs* mxfs);

void mxfs_inode_zone_index_to_block_offset(uint64_t inode_index, uint32_t* block_index, uint8_t* offset);

int64_t mxfs_inode_zone_init_inode(
    ramdisk* disk, mx_superblock* superblock, uint64_t index, mx_disk_inode* inode);
int64_t mxfs_inode_zone_clear_inode(ramdisk* disk, mx_superblock* superblock, uint64_t index);

#endif
