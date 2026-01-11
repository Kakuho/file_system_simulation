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
  if(mxfs_init_root_directory(mxfs, disk) != 0){
    return;
  }
  return;
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
  // initialise the root directory
  char buffer[MX_BLOCKSIZE];
  memset(buffer, 0, MX_BLOCKSIZE);

  mx_disk_inode root_inode;
  memset(&root_inode, 0, sizeof(mx_disk_inode));
  root_inode.mode = MX_INODE_DIR;
  root_inode.size = MX_BLOCKSIZE;
  root_inode.blocks[0] = ROOT_BLOCK;

  mx_dirent* entries = (mx_dirent*)&buffer;
  entries[0].inode_num = A_INODE;
  memcpy(entries[0].name, "a.txt", 6);
  entries[1].inode_num = B_INODE;
  memcpy(entries[1].name, "b.txt", 6);
  entries[2].inode_num = C_INODE;
  memcpy(entries[2].name, "c.txt", 6);

  assert(mxfs_set_inode(mxfs, disk, 0, &root_inode) == 0);
  assert(mxfs_set_block(mxfs, disk, 0, buffer) == 0);

  assert(mxfs_get_inode(mxfs, disk, 0, &root_inode) == 0);
  assert(root_inode.mode == MX_INODE_DIR);
  assert(root_inode.size == MX_BLOCKSIZE);
  assert(root_inode.blocks[0] == ROOT_BLOCK);

  // a.txt initialisation

  mx_disk_inode a_inode;
  memset(&a_inode, 0, sizeof(mx_disk_inode));
  a_inode.mode = MX_INODE_FILE;
  a_inode.size = MX_BLOCKSIZE;
  a_inode.blocks[0] = A_BLOCK;

  memset(&buffer, 0, MX_BLOCKSIZE);
  memcpy(&buffer, "this is a.txt", 14);

  assert(mxfs_set_inode(mxfs, disk, A_INODE, &a_inode) == 0);
  assert(mxfs_set_block(mxfs, disk, A_BLOCK, buffer) == 0);

  assert(mxfs_get_inode(mxfs, disk, A_INODE, &a_inode) == 0);
  assert(a_inode.mode == MX_INODE_FILE);
  assert(a_inode.size == MX_BLOCKSIZE);
  assert(a_inode.blocks[0] == A_BLOCK);

  // b.txt initialisation

  mx_disk_inode b_inode;
  memset(&b_inode, 0, sizeof(mx_disk_inode));
  b_inode.mode = MX_INODE_FILE;
  b_inode.size = MX_BLOCKSIZE;
  b_inode.blocks[0] = B_BLOCK;

  memset(&buffer, 0, MX_BLOCKSIZE);
  memcpy(&buffer, "this is b.txt bye bye", 23);

  assert(mxfs_set_inode(mxfs, disk, B_INODE, &b_inode) == 0);
  assert(mxfs_set_block(mxfs, disk, B_BLOCK, buffer) == 0);

  assert(mxfs_get_inode(mxfs, disk, B_INODE, &b_inode) == 0);
  assert(b_inode.mode == MX_INODE_FILE);
  assert(b_inode.size == MX_BLOCKSIZE);
  assert(b_inode.blocks[0] == B_BLOCK);

  // c.txt initialisation

  mx_disk_inode c_inode;
  memset(&c_inode, 0, sizeof(mx_disk_inode));
  c_inode.mode = MX_INODE_FILE;
  c_inode.size = MX_BLOCKSIZE;
  c_inode.blocks[0] = C_BLOCK;

  memset(&buffer, 0, MX_BLOCKSIZE);
  memcpy(&buffer, "see you c.txt", 14);

  assert(mxfs_set_inode(mxfs, disk, C_INODE, &c_inode) == 0);
  assert(mxfs_set_block(mxfs, disk, C_BLOCK, buffer) == 0);

  assert(mxfs_get_inode(mxfs, disk, C_INODE, &c_inode) == 0);
  assert(c_inode.mode == MX_INODE_FILE);
  assert(c_inode.size == MX_BLOCKSIZE);
  assert(c_inode.blocks[0] == C_BLOCK);

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

RSTATUS mxfs_get_inode(mxfs* mxfs, ramdisk* disk, uint64_t index, mx_disk_inode* buffer){
  if(mxfs_inode_zone_get_inode(mxfs, disk, index, buffer) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_set_inode(mxfs* mxfs, ramdisk* disk, uint64_t index, mx_disk_inode* inode){
  if(mxfs_inode_bitmap_set(disk, &mxfs->superblock, index) != 0){
    return -1;
  }
  if(mxfs_inode_zone_set_inode(mxfs, disk, index, inode) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_clear_inode(mxfs* mxfs, ramdisk* disk, uint64_t index){
  if(mxfs_inode_bitmap_clear(disk, &mxfs->superblock, index) != 0){
    return -1;
  }
  if(mxfs_inode_zone_clear_inode(mxfs, disk, index) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_set_block(mxfs* mxfs, ramdisk* disk, uint64_t index, char* buffer){
  if(mxfs_block_bitmap_set(disk, mxfs, index) != 0){
    return -1;
  }
  if(mxfs_block_zone_set_block(mxfs, disk, index, buffer) != 0){
    return -1;
  }
  return 0;
}

RSTATUS mxfs_clear_block(mxfs* mxfs, ramdisk* disk, uint64_t index){
  if(mxfs_block_bitmap_clear(disk, mxfs, index) != 0){
    return -1;
  }
  if(mxfs_block_zone_clear_block(mxfs, disk, index) != 0){
    return -1;
  }
  return 0;
}
