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
