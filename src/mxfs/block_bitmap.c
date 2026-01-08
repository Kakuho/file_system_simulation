#include "block_bitmap.h"

RSTATUS mxfs_block_bitmap_init(mxfs* mxfs, ramdisk* disk, uint16_t base, uint64_t nblocks){
  if(mxfs_block_bitmap_setup_memory(disk, base, nblocks) != 0){
    return -1;
  }
  if(mxfs_block_bitmap_register(mxfs, base, nblocks) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_block_bitmap_setup_memory(ramdisk* disk, uint16_t base, uint64_t nblocks){
  uint32_t bytes = nblocks/8; // 1 block = 1 bit
  uint32_t blocks = bytes/MX_BLOCKSIZE;

  char buffer[MX_BLOCKSIZE];
  memset(buffer, 0, MX_BLOCKSIZE);
  int i;
  for(i = 0; i < blocks; i++){
    if(ramdisk_write(disk, buffer, base + i) == -1){
      return -1;
    }
  }
  return 0;
}

RSTATUS mxfs_block_bitmap_register(mxfs* mxfs, uint16_t base, uint64_t nblocks){
  // base is the base index of the bitmap
  if(mxfs == NULL){
    return -1;
  }
  mxfs->superblock.nblocks = nblocks;
  mxfs->superblock.block_bitmap_base = base;
  mxfs->superblock.blocks_used = 0;
  // bitmap_base_index + bitmap_blocks
  uint32_t bytes = nblocks/8; 
  uint32_t blocks = bytes/MX_BLOCKSIZE;
  mxfs->superblock.block_base = base + blocks;
  return 0;
}

size_t mxfs_block_bitmap_nblocks(mxfs* mxfs){
  if(mxfs == NULL){
    return -1;
  }
  mx_superblock* superblock = &mxfs->superblock;
  size_t bytes = superblock->nblocks / 8;
  return bytes/MX_BLOCKSIZE;
}
