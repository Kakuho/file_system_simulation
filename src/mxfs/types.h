#ifndef MX_TYPES_H
#define MX_TYPES_H

#define MX_BLOCKSIZE 1024

#define MX_INODE_INDIRECT_ENTRIES 9    // 7 single, 1 double, 1 triple
#define MX_INODE_DOUBLE_INDIRECT 7
#define MX_INODE_TRIPLE_INDIRECT 8

#define MX_SUPERBLOCK_INDEX 1

#include <stddef.h>
#include <stdint.h>

//  The organisation of the file system:
//  block_1     | block_2            | block_3 ... block_m            | block_m           | block_m+1 ... block_n
//  superblock  | inodebitmap block    blocks consisting of inodes      datablock bitmap    blocks for data blocks

typedef struct mx_superblock{
  uint16_t ninodes;             // 65535 file inodes, plenty
  uint16_t inodes_used;
  uint16_t inode_bitmap_base;
  uint16_t inode_base; 
  uint16_t nblocks;             // data blocks
  uint16_t block_bitmap_base;
  uint16_t block_base;
} mx_superblock;

typedef struct mx_disk_inode{
  uint16_t mode;
  size_t size;
  uintptr_t blocks[MX_INODE_INDIRECT_ENTRIES];
} mx_disk_inode;

typedef struct mx_data_block{
  uint8_t data[MX_BLOCKSIZE];
} mx_data_block;

// should be a compile time function / constant
inline unsigned mx_inodes_per_block(){ return MX_BLOCKSIZE/sizeof(mx_disk_inode); }

#endif
