#include "ramdisk.h"
#include "mxfs/mxfs.h"
#include "assert.h"

void sample_ramdisk_10(){
  ramdisk* disk = ramdisk_create(5, 10);
  if(disk == NULL){
    return;
  }

  ramdisk_print_properties(disk);
  ramdisk_print(disk);
  char write[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  if(ramdisk_write(disk, write, 1) != 0){
    ramdisk_destroy(disk);
    return;
  }
  printf("Successful Write\n");
  ramdisk_print_properties(disk);
  ramdisk_print(disk);

  ramdisk_destroy(disk);
}

void sample_mxfs_inode_bitmap(){
  mxfs mxfs;
  ramdisk* disk = ramdisk_create(100, 1024);
  if(disk == NULL){
    return;
  }
  ramdisk_print_properties(disk);
  ramdisk_print_block(disk, 1);
  mxfs_init(&mxfs, disk, 100, 100);
  ramdisk_print_block(disk, 1);
  int i;
  for(i = 0; i < 16; i++){
    mx_inode_bitmap_allocate(disk);
  }
  char superblock_buffer[MX_BLOCKSIZE];
  if(mxfs_read_superblock(disk, superblock_buffer) != 0){
    return;
  }
  mx_superblock* sp = (mx_superblock*)superblock_buffer;
  ramdisk_dump(disk, "dump.txt");
  ramdisk_print_block(disk, 1);
  ramdisk_print_block(disk, 1);
  assert(sp->ninodes == 100);
  assert(sp->inodes_used == 16);
}

int main(){
  sample_mxfs_inode_bitmap();
}
