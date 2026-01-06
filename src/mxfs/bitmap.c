#include "bitmap.h"

RSTATUS mx_bitmap_init(mx_bitmap* bitmap, ramdisk* disk, uint16_t base, uint64_t length){
  if(disk == NULL){
    return -1;
  }
  if(disk->blocksize != MX_BLOCKSIZE){
    return -1;
  }
  bitmap->base = base;
  bitmap->length = length;
  // initialising the bitmap onto the disk
  size_t bytes = length / 8;
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

size_t mx_bitmap_nblocks(mx_bitmap* bitmap){
  if(bitmap == NULL){
    return -1;
  }
  size_t bytes = bitmap->length / 8;
  return bytes/MX_BLOCKSIZE;
}

RSTATUS mx_bitmap_register_inodemap(mx_bitmap* inodebitmap, mx_superblock* superblock){
  if(inodebitmap == NULL || superblock == NULL){
    return -1;
  }
  superblock->ninodes = inodebitmap->length;
  superblock->inode_bitmap_base = inodebitmap->base;
  // bitmap_base_index + bitmap_blocks
  size_t blocks = mx_bitmap_nblocks(inodebitmap);
  superblock->inode_base = inodebitmap->base + blocks;
  return 0;
}

RSTATUS mx_bitmap_register_blockmap(mx_bitmap* blockmap, mx_superblock* superblock){
  if(blockmap == NULL || superblock == NULL){
    return -1;
  }
  superblock->nblocks = blockmap->length;
  superblock->block_bitmap_base = blockmap->base;
  // bitmap_base_index + bitmap_blocks
  size_t blocks = mx_bitmap_nblocks(blockmap);
  superblock->block_base = blockmap->base + blocks;
  return 0;
}

size_t mx_inode_bitmap_nblocks(mx_superblock* superblock){
  if(superblock == NULL){
    return -1;
  }
  size_t bytes = superblock->ninodes / 8;
  return bytes/MX_BLOCKSIZE;
}

void mx_inode_index_to_byte_offset(
  uint64_t inode_index, 
  uint32_t* byte_index, 
  uint8_t* offset
){
  *byte_index = inode_index / 8;
  *offset = inode_index - ((inode_index/8) * 8);
}

int64_t mx_inode_get_free_index(char* inode_block, mx_superblock* superblock){
  // we know a block is 1024 bytes long, therefore the bitmap is 1024*8 bits long
  // remember 0 as free and 1 as busy
  uint8_t mask = 0xFF;
  int i;
  for(i = 0; i < mx_inode_bitmap_nblocks(superblock); i++){
    uint8_t present_mask = ~(inode_block[i] & mask);  // 0 for busy 1 for free
    if(present_mask > 0){
      int bitpos = 0;  // counting from the right-most bits of the present_mask
      while(!(present_mask >> bitpos & 0x01)){
        bitpos++;
      }
      return i*8 + (7 - bitpos);
    }
  }
  return -1;
}

int64_t mx_inode_set(ramdisk* disk, mx_superblock* superblock, uint64_t index){
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
  buffer[byteno] |= 1 << (7 - offset);
  ramdisk_write(disk, buffer, superblock->inode_bitmap_base);
  return 0;
}

int64_t mx_inode_clear(ramdisk* disk, mx_superblock* superblock, uint64_t index){
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

void mx_inode_bitmap_allocate(ramdisk* disk){
  // first read the superblock into ram 
  char sb_buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
  mx_superblock* superblock = (mx_superblock*)sb_buffer;
  // then find a free index from the bitmap and set it
  char buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, buffer, superblock->inode_bitmap_base);
  int64_t index = mx_inode_get_free_index(buffer, superblock);
  mx_inode_set(disk, superblock, index);
  // finally update the super block
  superblock->ninodes = superblock->ninodes-1;
  ramdisk_write(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
}

void mx_inode_bitmap_deallocate(ramdisk* disk, int64_t index){
  // first read the superblock into ram 
  char sb_buffer[MX_BLOCKSIZE];
  ramdisk_read(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
  mx_superblock* superblock = (mx_superblock*)sb_buffer;
  mx_inode_clear(disk, superblock, index);
  // finally update the super block
  superblock->ninodes = superblock->ninodes+1;
  ramdisk_write(disk, sb_buffer, MX_SUPERBLOCK_INDEX);
}
