#ifndef MXFS_BLOCK_ZONE_H
#define MXFS_BLOCK_ZONE_H

#include <assert.h>

#include "types.h"
#include "mxfs.h"
#include "ramdisk.h"

RSTATUS mxfs_block_zone_init(mxfs* mxfs, ramdisk* disk);
RSTATUS mxfs_block_zone_setup_memory(mxfs* mxfs, ramdisk* disk);

RSTATUS mxfs_block_zone_poison(mxfs* mxfs, ramdisk* disk, char ch);

size_t mxfs_block_zone_nblocks(mxfs* mxfs);

void mxfs_block_zone_index_to_disk_index(mxfs* mxfs, uint64_t index, uint32_t* disk_index);

int64_t mxfs_block_zone_set_block(mxfs* mxfs, ramdisk* disk, uint64_t index, char* buffer);
int64_t mxfs_block_zone_clear_block(mxfs* mxfs, ramdisk* disk, uint64_t index);

#endif
