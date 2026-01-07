#include "mxfs.h"
#include "types.h"
#include "bitmap.h"

void mxfs_init(ramdisk* disk, size_t ninodes, size_t nblocks){
  mx_superblock* superblock = (mx_superblock*)malloc(sizeof(mx_superblock));
  if(superblock == NULL){
    // ABORT!!!
    return;
  }
  // initialise the ninodes
  mx_bitmap inodebitmap;
  if(mx_bitmap_init(&inodebitmap, disk, MX_SUPERBLOCK_INDEX + 1, ninodes) != 0){
    // ABORT!!
    return;
  }
  if(mx_bitmap_register_inodemap(&inodebitmap, superblock) != 0){
    // ABORT!!
    return;
  }
  // initialise the blocks
  mxfs_flush_superblock(disk, superblock);
  free(superblock);
}

void mxfs_flush_superblock(ramdisk* disk, mx_superblock* superblock){
  char buffer[MX_BLOCKSIZE];
  memcpy(buffer, superblock, MX_BLOCKSIZE);
  if(ramdisk_write(disk, buffer, MX_SUPERBLOCK_INDEX) != 0){
    return;
  }
}

RSTATUS mxfs_read_superblock(ramdisk* disk, char* buffer){
  if(ramdisk_read(disk, buffer, MX_SUPERBLOCK_INDEX) != 0){
    return -1;
  }
  return 0;
}
