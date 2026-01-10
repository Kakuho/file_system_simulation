#include "block_zone.h"

RSTATUS mxfs_block_zone_init(mxfs* mxfs, ramdisk* disk){
  if(mxfs_block_zone_setup_memory(mxfs, disk) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_block_zone_setup_memory(mxfs* mxfs, ramdisk* disk){
  if(disk == NULL){
    return -1;
  }
  if(disk->blocksize != MX_BLOCKSIZE){
    return -1;
  }
  size_t nblocks = mxfs->superblock.nblocks;
  if(nblocks == 0){
    return 0;
  }
  // initialising the data blocks of the disk
  size_t blocks = mxfs->superblock.nblocks;
  unsigned bzone_base = mxfs->superblock.block_base;
  char buffer[MX_BLOCKSIZE];
  memset(buffer, 0, MX_BLOCKSIZE);
  unsigned i = 0;
  for(i = 0; i < blocks; i++){ 
    if(ramdisk_write(disk, buffer, bzone_base + i) == -1){
      return -1;
    }
  }
  return 0;
}

RSTATUS mxfs_block_zone_poison(mxfs* mxfs, ramdisk* disk, char ch){
  if(mxfs == NULL | disk == NULL){
    return -1;
  }
  if(disk->blocksize != MX_BLOCKSIZE){
    return -1;
  }
  size_t nblocks = mxfs_block_zone_nblocks(mxfs);
  unsigned bzone_base = mxfs->superblock.block_base;
  char block_buffer[MX_BLOCKSIZE];
  unsigned i;
  for(i = 0; i < nblocks; i++){
    memset(block_buffer, ch, MX_BLOCKSIZE);
    if(ramdisk_write(disk, block_buffer, bzone_base + i) == -1){
      return -1;
    }
  }
  return 0;
}

size_t mxfs_block_zone_nblocks(mxfs* mxfs){
  return mxfs->superblock.nblocks;
}

void mxfs_block_zone_index_to_disk_index(
  mxfs* mxfs, 
  uint64_t index,
  uint32_t* disk_index
){
  assert(index < mxfs->superblock.nblocks);
  *disk_index = index + mxfs->superblock.block_base;
}

int64_t mxfs_block_zone_set_block(
  mxfs* mxfs,
  ramdisk* disk, 
  uint64_t index, 
  char* buffer
){
  assert(index < mxfs->superblock.nblocks);
  uint32_t disk_index;
  mxfs_block_zone_index_to_disk_index(mxfs, index, &disk_index);
  if(ramdisk_write(disk, buffer, disk_index) != 0){
    return -1;
  }
  return 0;

}

int64_t mxfs_block_zone_clear_block(
  mxfs* mxfs,
  ramdisk* disk, 
  uint64_t index
){
  assert(index < mxfs->superblock.nblocks);
  uint32_t disk_index;
  mxfs_block_zone_index_to_disk_index(mxfs, index, &disk_index);
  char buffer[MX_BLOCKSIZE];
  memset(buffer, 0, MX_BLOCKSIZE);
  if(ramdisk_write(disk, buffer, disk_index) != 0){
    return -1;
  }
  return 0;
}
