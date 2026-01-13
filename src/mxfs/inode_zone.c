#include "inode_zone.h"


RSTATUS mxfs_inode_zone_init(mxfs* mxfs, ramdisk* disk){
  if(mxfs_inode_zone_setup_memory(mxfs, disk) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_inode_zone_setup_memory(mxfs* mxfs, ramdisk* disk){
  if(disk == NULL){
    return -1;
  }
  if(disk->blocksize != MX_BLOCKSIZE){
    return -1;
  }
  size_t ninodes = mxfs->superblock.ninodes;
  if(ninodes == 0){
    return 0;
  }
  // initialising the inode blocks of the disk
  size_t bytes = ninodes * sizeof(mx_disk_inode);
  size_t blocks = bytes/MX_BLOCKSIZE + 1;
  unsigned izone_base = mxfs->superblock.inode_base;
  char block_buffer[MX_BLOCKSIZE];
  memset(block_buffer, 0, MX_BLOCKSIZE);
  unsigned i = 0;
  for(i = 0; i < blocks; i++){ 
    if(ramdisk_write(disk, block_buffer, izone_base + i) == -1){
      return -1;
    }
  }
  return 0;
}

RSTATUS mxfs_inode_zone_poison(mxfs* mxfs, ramdisk* disk, char ch){
  if(mxfs == NULL | disk == NULL){
    return -1;
  }
  if(disk->blocksize != MX_BLOCKSIZE){
    return -1;
  }
  // initialising the inode blocks of the disk
  size_t nblocks = mxfs_inode_zone_nblocks(mxfs);
  unsigned izone_base = mxfs->superblock.inode_base;
  size_t byte_count = mxfs->superblock.ninodes * sizeof(mx_disk_inode);  // what about alignment bro
  printf("size of disk inode %lu\n", sizeof(mx_disk_inode));
  printf("total bytes of inode zone %lu\n", byte_count);
  unsigned block_index = izone_base;
  char block_buffer[MX_BLOCKSIZE];
  while(byte_count > 0){
    // would it be better to read the disk block in first and then perform the poisoning?
    if(byte_count > MX_BLOCKSIZE){
      memset(block_buffer, ch, MX_BLOCKSIZE);
      byte_count -= MX_BLOCKSIZE;
    }
    else{
      memset(block_buffer, ch, byte_count);
      byte_count -= byte_count;
    }
    if(ramdisk_write(disk, block_buffer, block_index) == -1){
      return -1;
    }
    block_index++;
    memset(block_buffer, 0, MX_BLOCKSIZE);  
  }
  return 0;
}

size_t mxfs_inode_zone_nblocks(mxfs* mxfs){
  size_t ninodes = mxfs->superblock.ninodes;
  if(ninodes == 0){
    return 0;
  }
  uintptr_t inodezone_base = mxfs->superblock.inode_base;
  size_t bytes = ninodes * sizeof(mx_disk_inode);
  size_t blocks = bytes/MX_BLOCKSIZE + 1;
  return blocks;
}

void mxfs_inode_zone_index_to_block_offset(
  mxfs* mxfs, 
  uint64_t inode_index,
  uint32_t* block_index, 
  uint8_t* offset
){
  // given an inode index, we need to figure out which data block on the disk it resides, and what its
  // offset within the data block is
  assert(inode_index < mxfs->superblock.ninodes);
  uint64_t global_byte_offset = inode_index * sizeof(mx_disk_inode);
  *block_index = global_byte_offset/MX_BLOCKSIZE;
  *offset = global_byte_offset - *block_index * MX_BLOCKSIZE;
  *block_index += mxfs->superblock.inode_base;
  debug_log("block index: %u\n", *block_index);
  debug_log("offset: %u", *offset);
}

int64_t mxfs_inode_zone_get_inode(
  mxfs* mxfs,
  ramdisk* disk, 
  uint64_t index, 
  mx_disk_inode* inode
){
  assert(index < mxfs->superblock.ninodes);
  uint32_t blockindex;
  uint8_t offset;
  mxfs_inode_zone_index_to_block_offset(mxfs, index, &blockindex, &offset);
  char buffer[MX_BLOCKSIZE];
  if(ramdisk_read(disk, buffer, blockindex) != 0){
    return -1;
  }
  // now we can perform the init, simply perform a copy of the memory
  memcpy(inode, &buffer[offset], sizeof(mx_disk_inode));
  return 0;
}

int64_t mxfs_inode_zone_set_inode(
  mxfs* mxfs,
  ramdisk* disk, 
  uint64_t index, 
  mx_disk_inode* inode
){
  assert(index < mxfs->superblock.ninodes);
  uint32_t blockindex;
  uint8_t offset;
  mxfs_inode_zone_index_to_block_offset(mxfs, index, &blockindex, &offset);
  char buffer[MX_BLOCKSIZE];
  if(ramdisk_read(disk, buffer, blockindex) != 0){
    return -1;
  }
  // now we can perform the init, simply perform a copy of the memory
  memcpy(&buffer[offset], inode, sizeof(mx_disk_inode));
  if(ramdisk_write(disk, buffer, blockindex) != 0){
    return -1;
  }
  return 0;
}

int64_t mxfs_inode_zone_clear_inode(
  mxfs* mxfs,
  ramdisk* disk, 
  uint64_t index
){
  assert(index < mxfs->superblock.ninodes);
  uint32_t blockindex;
  uint8_t offset;
  mxfs_inode_zone_index_to_block_offset(mxfs, index, &blockindex, &offset);
  char buffer[MX_BLOCKSIZE];
  if(ramdisk_read(disk, buffer, blockindex) != 0){
    return -1;
  }
  // now we can perform the clear, simply perform a copy of the memory
  memset(&buffer[offset], 0, sizeof(mx_disk_inode));
  if(ramdisk_write(disk, buffer, blockindex) != 0){
    return -1;
  }
  return 0;
}
