#ifndef MINIX_TYPES_H
#define MINIX_TYPES_H

#define MINIX_BLOCKSIZE 1024

typedef struct minix_superblock{
  unsigned short s_ninodes;
  unsigned short s_nzones;
  unsigned short s_imap_blocks;
  unsigned short s_zmap_blocks;
  unsigned short s_firstdatazone;
  unsigned short s_log_zone_size;
  unsigned long s_max_size;
  unsigned short s_magic;
} minix_superblock;

typedef struct minix_disk_inode{
  unsigned short i_mode;
  unsigned short i_uid;
  unsigned long  i_size;
  unsigned long  i_time;
  unsigned char  i_gid;
  unsigned char  i_nlinks;
  unsigned short i_zone[7+1+1];
} minix_disk_inode;

#endif
