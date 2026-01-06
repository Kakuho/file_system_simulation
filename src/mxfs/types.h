#ifndef MX_TYPES_H
#define MX_TYPES_H

#define MX_BLOCKSIZE 1024

typedef struct mx_superblock{
  unsigned short s_ninodes;
  unsigned short s_nzones;
  unsigned short s_imap_blocks;
  unsigned short s_zmap_blocks;
  unsigned short s_firstdatazone;
  unsigned short s_log_zone_size;
  unsigned long s_max_size;
  unsigned short s_magic;
} mx_superblock;

typedef struct mx_disk_inode{
  unsigned short i_mode;
  unsigned short i_uid;
  unsigned long  i_size;
  unsigned long  i_time;
  unsigned char  i_gid;
  unsigned char  i_nlinks;
  unsigned short i_zone[7+1+1];
} mx_disk_inode;

#endif
