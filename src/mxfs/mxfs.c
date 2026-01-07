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

RSTATUS mxfs_flush_superblock(mxfs* mxfs, ramdisk* disk){
  char buffer[MX_BLOCKSIZE];
  memcpy(buffer, &mxfs->superblock, MX_BLOCKSIZE);
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
  memcpy(&mxfs->superblock, buffer, MX_BLOCKSIZE);
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
