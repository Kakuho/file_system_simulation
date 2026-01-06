#ifndef MX_BITMAP_H
#define MX_BITMAP_H

#include <stdint.h>

#include "ramdisk.h"
#include "types.h"
#include "string.h"

#include "mxfs/types.h"

#define MXBITMAP_FREE 0
#define MXBITMAP_BUSY 1

typedef struct mx_bitmap{
  uint16_t base;      // index of the disk block which contains the base of the bitmap
  uint64_t length;    // num of bits 
} mx_bitmap;

RSTATUS mx_bitmap_init(mx_bitmap* bitmap, ramdisk* disk, uint16_t base, uint64_t length);

size_t mx_bitmap_nblocks(mx_bitmap* bitmap);

RSTATUS mx_bitmap_register_inodemap(mx_bitmap* inodebitmap, mx_superblock* superblock);
RSTATUS mx_bitmap_register_blockmap(mx_bitmap* blockmap, mx_superblock* superblock);

void mx_inode_bitmap_allocate(mx_superblock* superblock);
void mx_inode_bitmap_deallocate(mx_superblock* superblock);

void mx_block_bitmap_allocate(mx_superblock* superblock);
void mx_block_bitmap_deallocate(mx_superblock* superblock);

#endif
