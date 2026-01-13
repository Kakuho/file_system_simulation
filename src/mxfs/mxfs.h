#ifndef MXFS_MXFS_H
#define MXFS_MXFS_H

#include "ramdisk.h"
#include "bitmap.h"
#include "types.h"
#include "path_indexer.h"
#include "debug/debug_log.h"

typedef struct mxfs{
  mx_bitmap inode_bitmap;
  mx_bitmap block_bitmap; 
  mx_superblock superblock;
} mxfs;

typedef struct component_buffer{
  char buffer[100];
  int32_t length;
} component_buffer;

void mxfs_init(mxfs* mxfs, ramdisk* disk, size_t ninodes, size_t nblocks);
// initially root directory should look like:
//    /
//      a.txt       1, a.txt
//      b.txt       2, b.txt
//      c.txt       3, c.txt
RSTATUS mxfs_init_root_directory(mxfs* mxfs, ramdisk* disk);

RSTATUS mxfs_flush_superblock(mxfs* mxfs, ramdisk* disk);
RSTATUS mxfs_refresh_superblock(mxfs* mxfs, ramdisk* disk);

RSTATUS mxfs_write_superblock(ramdisk* disk, char* buffer);
RSTATUS mxfs_read_superblock(ramdisk* disk, char* buffer);

RSTATUS mxfs_get_inode(mxfs* mxfs, ramdisk* disk, uint64_t index, mx_disk_inode* buffer);
RSTATUS mxfs_set_inode(mxfs* mxfs, ramdisk* disk, uint64_t index, mx_disk_inode* inode);
RSTATUS mxfs_clear_inode(mxfs* mxfs, ramdisk* disk, uint64_t index);

RSTATUS mxfs_set_block(mxfs* mxfs, ramdisk* disk, uint64_t index, char* buffer);
RSTATUS mxfs_clear_block(mxfs* mxfs, ramdisk* disk, uint64_t index);

RSTATUS mxfs_read_inode_block(
  mxfs* mxfs, ramdisk* disk, mx_disk_inode* inode, uint64_t blockno, char* buffer);

int32_t mxfs_dirent_to_inode(mxfs* mxfs, ramdisk* disk, uint64_t dir_index, component_buffer* buffer);
int32_t mxfs_path_to_inode(mxfs* mxfs, ramdisk* disk, const char* path);

void mxfs_print_tree(mxfs* mxfs, ramdisk* disk);

void mxfs_create_file_rootdir(mxfs* mxfs, ramdisk* disk, const char* name);
void mxfs_create_file(mxfs* mxfs, ramdisk* disk, uint32_t pdir_inode, const char* name);

void mxfs_create_dir_rootdir(mxfs* mxfs, ramdisk* disk, const char* name);
void mxfs_create_dir(mxfs* mxfs, ramdisk* disk, uint32_t pdir_inode, const char* name);

#endif
