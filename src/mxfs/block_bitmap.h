#ifndef MXFS_BLOCK_BITMAP_H
#define MXFS_BLOCK_BITMAP_H

// manager class for the disk block bitmap

#include <stdint.h>

#include "types.h"
#include "ramdisk.h"

#include "mxfs.h"

RSTATUS mxfs_block_bitmap_init(mxfs* mxfs, ramdisk* disk, uint16_t base, uint64_t nblocks);
RSTATUS mxfs_block_bitmap_setup_memory(ramdisk* disk, uint16_t base, uint64_t nblocks);
RSTATUS mxfs_block_bitmap_register(mxfs* mxfs, uint16_t base, uint64_t nblocks);

RSTATUS mxfs_block_bitmap_poison(mxfs* mxfs, ramdisk* disk, char ch);

size_t mxfs_block_bitmap_nblocks(mxfs* mxfs);

int64_t mxfs_block_bitmap_get_free_index(ramdisk* disk, mxfs* mxfs);
int64_t mxfs_block_bitmap_block_free_index(char* block, mxfs* mxfs);

int64_t mxfs_block_bitmap_set(ramdisk* disk, mxfs* mxfs, uint64_t index);
int64_t mxfs_block_bitmap_clear(ramdisk* disk, mxfs* mxfs, uint64_t index);

void mxfs_block_bitmap_allocate(ramdisk* disk, mxfs* mxfs);
void mxfs_block_bitmap_deallocate(ramdisk* disk, mxfs* mxfs, int64_t index);

#endif
