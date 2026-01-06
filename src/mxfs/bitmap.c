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

int64_t mx_inode_get_free_index(char* inode_block, mx_superblock* superblock){
  // we know a block is 1024 bytes long, therefore the bitmap is 1024*8 bits long
  // remember 0 as free and 1 as busy
  uint8_t mask = 0xFF;
  int i;
  for(i = 0; i < mx_inode_bitmap_nblocks(superblock); i++){
    uint8_t present_mask = ~(inode_block[i] & mask);  // 0 for busy 1 for free
    if(present_mask > 0){
      int bitpos = 0;  // counting from the right most of the present_mask
      while(!(present_mask >> bitpos & 0x01)){
        bitpos++;
      }
      return i*8 + bitpos;
    }
  }
  return -1;
}

void mx_inode_bitmap_allocate(ramdisk* disk, mx_superblock* superblock){
  // first find a free index from the bitmap
  uint16_t bitmap_base = superblock->inode_bitmap_base;
  size_t inodeblocks = mx_inode_bitmap_nblocks(superblock);
  uint64_t index = 0;
  while(index < inodeblocks){
    // check if disk block i has a free entry
    char buffer[MX_BLOCKSIZE];
    ramdisk_read(disk, buffer, index);
    int i = mx_inode_get_free_index(buffer, superblock);
    index++;
  }

  // update the super block?
}
