#include "mxfs.h"
#include "types.h"
#include "bitmap.h"

void mxfs_setup_inodes_dblocks(mxfs* mxfs, ramdisk* disk, size_t ninodes, size_t nblocks){
  if(mxfs == NULL || disk == NULL){
    // ABORT!!!
    return;
  }
  // initialise the ninodes
  mx_bitmap inodebitmap;
  if(mx_bitmap_init(&inodebitmap, disk, MX_SUPERBLOCK_INDEX + 1, ninodes) != 0){
    // ABORT!!
    return;
  }
  if(mx_bitmap_register_inodemap(&inodebitmap, &mxfs->superblock) != 0){
    // ABORT!!
    return;
  }
}

void mxfs_init(mxfs* mxfs, ramdisk* disk, size_t ninodes, size_t nblocks){
  memset(&mxfs->superblock, 0, sizeof(mxfs->superblock));
  mxfs_setup_inodes_dblocks(mxfs, disk, ninodes, nblocks);
  // mxfs_setup_blocks
  if(mxfs_flush_superblock(mxfs, disk) != 0){
    // ABORT!!
    return;
  }
}

RSTATUS mxfs_flush_superblock(mxfs* mxfs, ramdisk* disk){
  char buffer[MX_BLOCKSIZE];
  memset(buffer, 0, MX_BLOCKSIZE);
  memcpy(buffer, &mxfs->superblock, sizeof(mx_superblock));
  if(ramdisk_write(disk, buffer, MX_SUPERBLOCK_INDEX) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_refersh_superblock(mxfs* mxfs, ramdisk* disk){
  char buffer[MX_BLOCKSIZE];
  if(ramdisk_read(disk, buffer, MX_SUPERBLOCK_INDEX) != 0){
    return -1;
  }
  memcpy(&mxfs->superblock, buffer, sizeof(mx_superblock));
  return 0;
}

RSTATUS mxfs_write_superblock(ramdisk* disk, char* buffer){
  if(ramdisk_write(disk, buffer, MX_SUPERBLOCK_INDEX) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_read_superblock(ramdisk* disk, char* buffer){
  if(ramdisk_read(disk, buffer, MX_SUPERBLOCK_INDEX) != 0){
    return -1;
  }
  return 0;
}
