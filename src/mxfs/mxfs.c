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
  root_inode.blocks[0] = 0;

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

static RSTATUS search_directory(
  mxfs* mxfs, 
  ramdisk* disk, 
  int64_t* inode_index, 
  char* string, 
  int32_t* length,
  int64_t* entry_inode
){
  // search the directory to see if it contains the string as an entry
  mx_disk_inode working_inode;
  //printf("\ninput inode: %ld\n", *inode_index);
  assert(mxfs_get_inode(mxfs, disk, *inode_index, &working_inode) == 0);
  char buffer[MX_BLOCKSIZE];
  //printf("working inode block 0: %ld", working_inode.blocks[0]);
  assert(working_inode.blocks[0] == 0);
  if(ramdisk_read(disk, buffer, working_inode.blocks[0] + mxfs->superblock.block_base) != 0){
    return -1;
  }
  // now we can finally loop through the disk block
  mx_dirent* entries = (mx_dirent*)buffer;
  uint32_t index = 0;
  while(index < MX_BLOCKSIZE / sizeof(mx_dirent)){
    if(memcmp(entries[index].name, string, *length) == 0){
      *entry_inode = entries[index].inode_num;
      return 0;
    }
    index++;
  }
  return -1;
}

RSTATUS mxfs_read_inode_block(mxfs* mxfs, ramdisk* disk, mx_disk_inode* inode, uint64_t blockno, char* buffer){
  uint64_t block_address = inode->blocks[blockno];
  if(ramdisk_read(disk, buffer, block_address) != 0){
    return -1;
  }
  return 0;
}

//  example paths:
//    .
//    a.txt
//    yaya/b.txt
//
//    /
//    /a.txt
//    /yaya/b.txt
//  for now it will assume absolute paths
int32_t mxfs_path_to_inode(mxfs* mxfs, ramdisk* disk, const char* path){
  path_indexer pidx;
  path_indexer_parse(&pidx, path);

  // required for walking the directory tree
  mx_disk_inode working_inode;
  char block_buffer[MX_BLOCKSIZE];
  char component_buffer[100];
  int64_t prev_inode_num = 0;
  int64_t inode_num = 0;
  int32_t length;
  uint32_t index = 0; // component index

  path_indexer_read_component(&pidx, 0, component_buffer, &length);
  assert(component_buffer[0] == '/');

  if(pidx.ncomponents == 1){
    return 0;
  }

  mxfs_get_inode(mxfs, disk, 0, &working_inode);
  assert(working_inode.mode == MX_INODE_DIR);
  index++;

  while(index < pidx.ncomponents){
    path_indexer_read_component(&pidx, index, component_buffer, &length);
    if((working_inode.mode != MX_INODE_DIR) && (index != pidx.ncomponents-1)){
      return -1;
    }
    // right now the data block is in the block buffer
    if(search_directory(mxfs, disk, &prev_inode_num, component_buffer, &length, &inode_num) != 0){
      return -1;
    }
    mxfs_get_inode(mxfs, disk, inode_num, &working_inode);
    prev_inode_num = inode_num;
    index++;
  }
  return inode_num;
}

static void print_file(
  mxfs* mxfs,
  ramdisk* disk,
  char* parent_buffer,
  uint32_t parent_length,
  char* component
){
  uint32_t comp_length = strlen(component);
  char path[100];
  memcpy(path, parent_buffer, parent_length);
  memcpy(&path[parent_length], component, comp_length);
  path[99] = '\0';
  printf("%s\n", path);
}

static void print_subdirectory(
  mxfs* mxfs, 
  ramdisk* disk, 
  uint32_t inode_num, 
  char* path, 
  uint32_t length
){
  mx_disk_inode working_inode;
  char block_buffer[MX_BLOCKSIZE];

  assert(mxfs_get_inode(mxfs, disk, inode_num, &working_inode) == 0);
  assert(working_inode.mode == MX_INODE_DIR);

  if(ramdisk_read(disk, block_buffer, working_inode.blocks[0] + mxfs->superblock.block_base) != 0){
   return;
  }

  uint32_t index = 0;
  mx_dirent* entries = (mx_dirent*)block_buffer;
  while(index < MX_BLOCKSIZE/sizeof(mx_dirent)){
    uint32_t inode_num = entries[index].inode_num;
    length += strlen(entries[index].name);
    if(inode_num != 0){
      mxfs_get_inode(mxfs, disk, inode_num, &working_inode);
      if(working_inode.mode == MX_INODE_FILE){
        print_file(mxfs, disk, path, length, entries[index].name);
      }
      else if(working_inode.mode == MX_INODE_DIR){
        print_subdirectory(mxfs, disk, inode_num, path, length);
      }
      else{
        assert((working_inode.mode == MX_INODE_DIR) || (working_inode.mode == MX_INODE_FILE));
      }
    }
    index++;
  }
}

void mxfs_print_tree(mxfs* mxfs, ramdisk* disk){
  //  print ordrer:
  //  /
  //  /yaya
  //  /yaya/a.txt
  //  /yaya/b.txt
  //  /yaya/b.txt
  //  /babaisyou
  //  /babaisyou/a.txt
  //  /babaisyou/b.txt
  //  /babaisyou/c.txt
  //
  //  read the root directory into ram
  //  go through the root directory and for each entry:
  //    if it is a directory
  //      read the directory into ram
  mx_disk_inode working_inode;
  char block_buffer[MX_BLOCKSIZE];
  char path_buffer[100];
  uint32_t length = 1;
  memset(path_buffer, 0, 100);
  path_buffer[0] = '/';
  printf("%s\n", path_buffer);

  mxfs_get_inode(mxfs, disk, 0, &working_inode);
  assert(working_inode.mode == MX_INODE_DIR);

  if(ramdisk_read(disk, block_buffer, working_inode.blocks[0] + mxfs->superblock.block_base) != 0){
   return;
  }

  uint32_t index = 0;
  mx_dirent* entries = (mx_dirent*)block_buffer;
  while(index < MX_BLOCKSIZE/sizeof(mx_dirent)){
    uint32_t inode_num = entries[index].inode_num;
    if(inode_num != 0){
      mxfs_get_inode(mxfs, disk, inode_num, &working_inode);
      if(working_inode.mode == MX_INODE_FILE){
        print_file(mxfs, disk, path_buffer, length, entries[index].name);
      }
      else if(working_inode.mode == MX_INODE_DIR){
        print_subdirectory(mxfs, disk, inode_num, path_buffer, length);
      }
      else{
        assert((working_inode.mode == MX_INODE_DIR) || (working_inode.mode == MX_INODE_FILE));
      }
    }
    index++;
  }
}
