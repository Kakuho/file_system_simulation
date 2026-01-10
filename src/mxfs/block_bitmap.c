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

RSTATUS mxfs_block_bitmap_poison(mxfs* mxfs, ramdisk* disk, char ch){
  if(mxfs == NULL | disk == NULL){
    return -1;
  }
  if(disk->blocksize != MX_BLOCKSIZE){
    return -1;
  }
  // initialising the inode blocks of the disk
  unsigned dbitmap_base = mxfs->superblock.block_bitmap_base;
  size_t byte_count = mxfs->superblock.nblocks/8 + 1;
  unsigned block_index = dbitmap_base;
  char block_buffer[MX_BLOCKSIZE];
  memset(block_buffer, 0, MX_BLOCKSIZE);
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

size_t mxfs_block_bitmap_nblocks(mxfs* mxfs){
  if(mxfs == NULL){
    return -1;
  }
  mx_superblock* superblock = &mxfs->superblock;
  size_t bytes = superblock->nblocks / 8;
  return bytes/MX_BLOCKSIZE;
}

// the block bitmap maybe more than 1024*8 bits long...
int64_t mxfs_block_bitmap_get_free_index(ramdisk* disk, mxfs* mxfs){
  uint32_t blocks = mxfs_block_bitmap_nblocks(mxfs);
  char bbuffer[MX_BLOCKSIZE];
  uint32_t i = 0;
  for(i = 0; i < blocks; i++){
    ramdisk_read(disk, bbuffer, mxfs->superblock.block_bitmap_base + i);
    int64_t index = mxfs_block_bitmap_block_free_index(bbuffer, mxfs);
    if(index != 0){
      return index;
    }
  }
  return -1;
}

int64_t mxfs_block_bitmap_block_free_index(char* block, mxfs* mxfs){
  // remember 0 as free and 1 as busy
  // find a free index in a single block
  uint8_t mask = 0xFF;
  uint32_t bitmap_bytes = mxfs->superblock.nblocks/8;
  int i;
  for(i = 0; i < bitmap_bytes; i++){
    uint8_t present_mask = ~(block[i] & mask);  // 0 for busy 1 for free
    if(present_mask > 0){
      int bitpos = 0;  // counting from the right-most bits of the present_mask
      while(!((present_mask >> bitpos) & 0x01)){
        bitpos++;
      }
      return i*8 + (7 - bitpos);
    }
  }
  return -1;
}

int64_t mxfs_block_bitmap_set(ramdisk* disk, mxfs* mxfs, uint64_t index){
  //  given the block index, set it to 1
  //  mapping of indexes to blocks:
  //    byte 0: 00 01 02 03 04 05 06 07
  //    byte 1: 08 09 10 11 12 13 14 15
  //    ...
  //    byte i: i*8 i*8+1 i*8+2 i*8+3 i*8+4 i*8+5 i*8+6 i*8+7
  uint32_t byteno = index / 8;
  uint32_t blockno = byteno / MX_BLOCKSIZE;
  char buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, buffer, mxfs->superblock.block_bitmap_base + blockno);
  uint32_t byte_offset = byteno - blockno * MX_BLOCKSIZE;
  uint32_t bit_offset = index - byteno * 8;
  // our index is at offset of the byte byteno
  // printf("byte index: %d\n", byteno);
  // printf("bit offset: %d\n", offset);
  buffer[byte_offset] = buffer[byte_offset] | (1 << (7 - bit_offset));
  ramdisk_write(disk, buffer, mxfs->superblock.block_bitmap_base + blockno);
  return 0;

}

int64_t mxfs_block_bitmap_clear(ramdisk* disk, mxfs* mxfs, uint64_t index){
  // given the block index, clear it to 0
  uint32_t byteno = index / 8;
  uint32_t blockno = byteno / MX_BLOCKSIZE;
  char buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, buffer, mxfs->superblock.block_bitmap_base + blockno);
  uint32_t byte_offset = byteno - blockno * MX_BLOCKSIZE;
  uint32_t bit_offset = index - byteno * 8;
  // our index is at offset of the byte byteno
  // printf("byte index: %d\n", byteno);
  // printf("bit offset: %d\n", offset);
  buffer[byte_offset] = buffer[byte_offset] & 0 << (7 - bit_offset);
  ramdisk_write(disk, buffer, mxfs->superblock.block_bitmap_base + blockno);
  return 0;
}

void mxfs_block_bitmap_allocate(ramdisk* disk, mxfs* mxfs){
  // first read the superblock into ram
  char sb_buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
  mx_superblock* superblock = (mx_superblock*)sb_buffer;
  // then find a free index from the bitmap and set it
  char buffer[MX_BLOCKSIZE];
  int64_t index = mxfs_block_bitmap_get_free_index(disk, mxfs);
  mxfs_block_bitmap_set(disk, mxfs, index);
  // finally update the super block
  superblock->blocks_used = superblock->blocks_used+1;
  ramdisk_write(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
}

void mxfs_block_bitmap_deallocate(ramdisk* disk, mxfs* mxfs, int64_t index){
  // first read the superblock into ram
  char sb_buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
  mx_superblock* superblock = (mx_superblock*)sb_buffer;
  mxfs_block_bitmap_clear(disk, mxfs, index);
  // finally update the super block
  superblock->blocks_used = superblock->inodes_used-1;
  ramdisk_write(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
}
