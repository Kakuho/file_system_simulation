#include "mxfs.h"
#include "inode_bitmap.h"
#include "inode_zone.h"
#include "block_bitmap.h"
#include "block_zone.h"

void mxfs_setup_inodes_dblocks(mxfs* mxfs, ramdisk* disk, size_t ninodes, size_t nblocks){
  if(mxfs == NULL || disk == NULL){
    // ABORT!!!
    return;
  }
  if(mxfs_inode_bitmap_init(mxfs, disk, MX_SUPERBLOCK_INDEX+1, ninodes) != 0){
    // ABORT!!
    return;
  }
  if(mxfs_inode_zone_init(mxfs, disk) != 0){
    // ABORT!!
    return;
  }
  uint16_t block_bitmap_base = mxfs->superblock.inode_base + mxfs_inode_zone_nblocks(mxfs);
  if(mxfs_block_bitmap_init(mxfs, disk, block_bitmap_base, nblocks) != 0){
    // ABORT!!
    return;
  }
  if(mxfs_block_zone_init(mxfs, disk) != 0){
    // ABORT!!
    return;
  }
  printf("Finished Initialising file system...\n");
  printf("Inode Bitmap Base Index: %d\n", MX_SUPERBLOCK_INDEX + 1);
  printf("Inode Bitmap Blocks: %lu\n", mxfs_inode_bitmap_nblocks(mxfs));
  printf("Inode Zone Base Index: %u\n", mxfs->superblock.inode_base);
  printf("Inode Zone Blocks: %lu\n", mxfs_inode_zone_nblocks(mxfs));
  printf("Block Bitmap Base Index: %d\n", mxfs->superblock.block_bitmap_base);
  printf("Block Bitmap Blocks: %lu\n", mxfs_block_bitmap_nblocks(mxfs));
  printf("Block Zone Base Index: %u\n", mxfs->superblock.block_base);
  printf("Block Zone Blocks: %lu\n", mxfs_block_zone_nblocks(mxfs));
}

void mxfs_init(mxfs* mxfs, ramdisk* disk, size_t ninodes, size_t nblocks){
  memset(&mxfs->superblock, 0, sizeof(mxfs->superblock));
  mxfs_setup_inodes_dblocks(mxfs, disk, ninodes, nblocks);
  // mxfs_setup_blocks
  if(mxfs_flush_superblock(mxfs, disk) != 0){
    // ABORT!!
    return;
  }
}

RSTATUS mxfs_init_root_directory_v1(mxfs* mxfs, ramdisk* disk){
  //  we want to set up the disk such that we get the following configuration:
  //    root('/')
  //      a.txt
  //      b.txt       
  //      c.txt     
  //
  //  physical structure:
  //    root directory - inode 0, block 0
  //      contents:
  //        1   a.txt
  //        2   b.txt
  //        3   c.txt
  //
  //    a.txt - inode 1, block 1
  //      contents:
  //        hello i am a
  //
  //    b.txt - inode 2, block 2
  //      contents:
  //        hello i am b
  //
  //    c.txt - inode 3, block 3
  //      contents:
  //        hello i am c
  //
  //  Improvements for later: 
  //    * for now the inode indicies and block numbers are hard coded, you should 
  //      make them variable 
  //    * allow a directories to be members of other directories
  #define ROOT_INODE 0
  #define A_INODE 1
  #define B_INODE 2
  #define C_INODE 3
  #define ROOT_BLOCK 0
  #define A_BLOCK 1
  #define B_BLOCK 2
  #define C_BLOCK 3
  // function to implement initialisation

  #undef ROOT_INODE 
  #undef A_INODE 
  #undef B_INODE 
  #undef C_INODE 
  #undef ROOT_BLOCK 
  #undef A_BLOCK 
  #undef B_BLOCK 
  #undef C_BLOCK 
  return 0;
}

RSTATUS mxfs_init_root_directory(mxfs* mxfs, ramdisk* disk){
  if(mxfs_init_root_directory_v1(mxfs, disk) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_flush_superblock(mxfs* mxfs, ramdisk* disk){
  char buffer[MX_BLOCKSIZE];
  memset(buffer, 0, MX_BLOCKSIZE);
  memcpy(buffer, &mxfs->superblock, sizeof(mx_superblock));
  if(ramdisk_write(disk, buffer, MX_SUPERBLOCK_INDEX) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_refresh_superblock(mxfs* mxfs, ramdisk* disk){
  char buffer[MX_BLOCKSIZE];
  if(ramdisk_read(disk, buffer, MX_SUPERBLOCK_INDEX) != 0){
    return -1;
  }
  memcpy(&mxfs->superblock, buffer, sizeof(mx_superblock));
  return 0;
}

RSTATUS mxfs_write_superblock(ramdisk* disk, char* buffer){
  if(ramdisk_write(disk, buffer, MX_SUPERBLOCK_INDEX) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_read_superblock(ramdisk* disk, char* buffer){
  if(ramdisk_read(disk, buffer, MX_SUPERBLOCK_INDEX) != 0){
    return -1;
  }
  return 0;
}
