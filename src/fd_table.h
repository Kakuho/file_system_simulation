#ifndef FD_TABLE_H
#define FD_TABLE_H

#define FDTABLE_NOT_PRESENT -1
#define FDTABLE_ENTRIES     10

#include "file_table.h"

// the per process file descriptor table

typedef struct file_descriptor{
  bool avail;
  open_file* fobj;
} file_descriptor;

bool fd_avail(file_descriptor* fd){
  if(fd->avail == true){ // is present
    return true;
  }
  else{
    return false;
  }
}

void fd_set_inuse(file_descriptor* fd){
  fd->avail = false;
}

// fd table

typedef struct fd_table{
  file_descriptor fds[10];
} fd_table;

int fd_table_allocate(fd_table* fdtbl){
  for(int i = 0; i < FDTABLE_ENTRIES; i++){
    if(fd_avail(&fdtbl->fds[i]) == true){
      fd_set_inuse(&fdtbl->fds[i]);
      return i;
    }
  }
  return -1;
}

#endif
