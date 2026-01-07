#ifndef MXFS_MXFS_H
#define MXFS_MXFS_H

#include "ramdisk.h"
#include "bitmap.h"
#include "types.h"

void mxfs_init(ramdisk* disk, size_t ninodes, size_t nblocks);
void mxfs_flush_superblock(ramdisk* disk, mx_superblock* superblock);

RSTATUS mxfs_read_superblock(ramdisk* disk, char* buffer);

#endif
