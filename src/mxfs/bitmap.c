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
