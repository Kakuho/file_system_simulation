#include "inode_bitmap.h"

RSTATUS mxfs_inode_bitmap_init(mxfs* mxfs, ramdisk* disk, uint16_t base, uint64_t ninodes){
  if(mxfs_inode_bitmap_setup_memory(disk, base, ninodes) != 0){
    return -1;
  }
  if(mxfs_inode_bitmap_register(mxfs, base, ninodes) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_inode_bitmap_setup_memory(ramdisk* disk, uint16_t base, uint64_t ninodes){
  if(disk == NULL){
    return -1;
  }
  if(disk->blocksize != MX_BLOCKSIZE){
    return -1;
  }
  // initialising the bitmap onto the disk
  size_t bytes = ninodes / 8;
  size_t blocks = bytes/MX_BLOCKSIZE;
  char block_buffer[MX_BLOCKSIZE];
  memset(block_buffer, MXBITMAP_FREE, MX_BLOCKSIZE);
  // write the bitmap data to the 
  unsigned i = 0;
  for(i = 0; i < blocks; i++){ 
    if(ramdisk_write(disk, block_buffer, base + i) == -1){
      return -1;
    }
  }
  return 0;

}

RSTATUS mxfs_inode_bitmap_register(mxfs* mxfs, uint16_t base, uint64_t ninodes){
  // base is the base index of the bitmap
  if(mxfs == NULL){
    return -1;
  }
  mxfs->superblock.ninodes = ninodes;
  mxfs->superblock.inode_bitmap_base = base;
  mxfs->superblock.inodes_used = 0;
  // bitmap_base_index + bitmap_blocks
  size_t blocks = mxfs_inode_bitmap_nblocks(mxfs);
  mxfs->superblock.inode_base = base + blocks;
  return 0;
}

RSTATUS mxfs_inode_bitmap_poison(mxfs* mxfs, ramdisk* disk, char ch){
  if(mxfs == NULL | disk == NULL){
    return -1;
  }
  if(disk->blocksize != MX_BLOCKSIZE){
    return -1;
  }
  // initialising the inode blocks of the disk
  unsigned ibitmap_base = mxfs->superblock.inode_bitmap_base;
  size_t byte_count = mxfs->superblock.ninodes/8 + 1;  // what about alignment bro
  unsigned block_index = ibitmap_base;
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

size_t mxfs_inode_bitmap_nblocks(mxfs* mxfs){
  mx_superblock* superblock = &mxfs->superblock;
  if(superblock == NULL){
    return -1;
  }
  size_t bytes = superblock->ninodes / 8;
  return bytes/MX_BLOCKSIZE + 1;
}

void mxfs_inode_bitmap_index_to_byte_offset(
  uint64_t inode_index,
  uint32_t* byte_index,
  uint8_t* offset
){
  *byte_index = inode_index / 8;
  *offset = inode_index - ((inode_index/8) * 8);
}

int64_t mxfs_inode_bitmap_get_free_index(char* inode_block, mx_superblock* superblock){
  // we know a block is 1024 bytes long, therefore the bitmap is 1024*8 bits long
  // remember 0 as free and 1 as busy
  uint8_t mask = 0xFF;
  int i;
  for(i = 0; i < superblock->ninodes; i++){
    uint8_t present_mask = ~(inode_block[i] & mask);  // 0 for busy 1 for free
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

int64_t mxfs_inode_bitmap_set(ramdisk* disk, mx_superblock* superblock, uint64_t index){
  //  given the inode index, set it to 1
  //  mapping of indexes to blocks:
  //    byte 0: 00 01 02 03 04 05 06 07
  //    byte 1: 08 09 10 11 12 13 14 15
  //    ...
  //    byte i: i*8 i*8+1 i*8+2 i*8+3 i*8+4 i*8+5 i*8+6 i*8+7
  //
  //  consumer grade
  char buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, buffer, superblock->inode_bitmap_base);
  uint32_t byteno = index / 8;
  uint8_t offset = index - ((index/8) * 8); // probably a better way to do this
  // our index is at offset of the byte byteno
  debug_log("byte index: %d\n", byteno);
  debug_log("offset: %d\n", offset);
  buffer[byteno] = buffer[byteno] | (1 << (7 - offset));
  ramdisk_write(disk, buffer, superblock->inode_bitmap_base);
  return 0;
}

int64_t mxfs_inode_bitmap_clear(ramdisk* disk, mx_superblock* superblock, uint64_t index){
  // given the inode index, clear it to 0
  // see mx_inode_set
  char buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, buffer, superblock->inode_bitmap_base);
  uint32_t byteno = index / 8;
  uint8_t offset = index - ((index/8) * 8); // probably a better way to do this
  // our index is at offset of the byte byteno
  buffer[byteno] &= 0 << (7 - offset);
  ramdisk_write(disk, buffer, superblock->inode_bitmap_base);
  return 0;
}

void mxfs_inode_bitmap_allocate(ramdisk* disk){
  // first read the superblock into ram
  char sb_buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
  mx_superblock* superblock = (mx_superblock*)sb_buffer;
  // then find a free index from the bitmap and set it
  char buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, buffer, superblock->inode_bitmap_base);
  int64_t index = mxfs_inode_bitmap_get_free_index(buffer, superblock);
  mxfs_inode_bitmap_set(disk, superblock, index); 
  // finally update the super block
  superblock->inodes_used = superblock->inodes_used+1;
  ramdisk_write(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
}

void mxfs_inode_bitmap_deallocate(ramdisk* disk, int64_t index){
  // first read the superblock into ram
  char sb_buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
  mx_superblock* superblock = (mx_superblock*)sb_buffer;
  mxfs_inode_bitmap_clear(disk, superblock, index);
  // finally update the super block
  superblock->inodes_used = superblock->inodes_used-1;
  ramdisk_write(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
}
