#include <assert.h>

#include "ramdisk.h"
#include "mxfs/mxfs.h"
#include "mxfs/inode_bitmap.h"
#include "mxfs/block_bitmap.h"
#include "mxfs/inode_zone.h"
#include "mxfs/block_zone.h"


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
  ramdisk_destroy(disk);
}

void sample_poison_mxfs(){
  mxfs mxfs;
  ramdisk* disk = ramdisk_create(140, 1024);
  if(disk == NULL){
    return;
  }
  mxfs_init(&mxfs, disk, 100, 100);
  mxfs_inode_bitmap_poison(&mxfs, disk, 0x88);
  mxfs_inode_zone_poison(&mxfs, disk, 0xBB);
  mxfs_block_bitmap_poison(&mxfs, disk, 0xFE);
  mxfs_block_zone_poison(&mxfs, disk, 0x24);
  ramdisk_dump(disk, "dump.txt");
  ramdisk_destroy(disk);
}

void sample_init_mxfs(){
  mxfs mxfs;
  ramdisk* disk = ramdisk_create(140, 1024);
  if(disk == NULL){
    return;
  }
  mxfs_init(&mxfs, disk, 100, 100);
  ramdisk_dump(disk, "dump.txt");
  ramdisk_destroy(disk);
}

int main(){
  sample_init_mxfs();
}
