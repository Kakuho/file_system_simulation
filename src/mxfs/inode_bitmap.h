#ifndef MXFS_INODE_BITMAP_H
#define MXFS_INODE_BITMAP_H

// manager class for the inode bitmap

#include <stdint.h>

#include "types.h"
#include "ramdisk.h"

#include "mxfs.h"

RSTATUS mxfs_inode_bitmap_init(mxfs* mxfs, ramdisk* disk, uint16_t base, uint64_t ninodes);
RSTATUS mxfs_inode_bitmap_setup_memory(ramdisk* disk, uint16_t base, uint64_t ninodes);
RSTATUS mxfs_inode_bitmap_register(mxfs* mxfs, uint16_t base, uint64_t ninodes);

size_t mxfs_inode_bitmap_nblocks(mxfs* mxfs);

void mxfs_inode_bitmap_index_to_byte_offset(
    uint64_t inode_index, uint32_t* byte_index, uint8_t* offset);

int64_t mxfs_inode_bitmap_get_free_index(char* inode_block, mx_superblock* superblock);

int64_t mxfs_inode_bitmap_set(ramdisk* disk, mx_superblock* superblock, uint64_t index);
int64_t mxfs_inode_bitmap_clear(ramdisk* disk, mx_superblock* superblock, uint64_t index);

void mxfs_inode_bitmap_allocate(ramdisk* disk);
void mxfs_inode_bitmap_deallocate(ramdisk* disk, int64_t index);

#endif
