#ifndef FILE_TABLE_H
#define FILE_TABLE_H

#define MAX_OFFSET          0xFFFFFF
#define FTABLE_NOT_PRESENT  MAX_OFFSET + 1
#define FTABLE_ENTRIES      10

#include <stdint.h>
#include "types.h"

// the global kernel open file object table

typedef struct open_file{
  // open file object
  // ig you can create a maximum file offset if you want?
  unsigned long file_offset;
  uint8_t flag; // present bit can be here
} open_file;

void open_file_clear(open_file* fobj){
  fobj->file_offset = FTABLE_NOT_PRESENT;
}

int open_file_not_present(const open_file* fobj){
  if(fobj->file_offset == FTABLE_NOT_PRESENT){
    return 0;
  }
  else{
    return -1;
  }
}

// file table

// array of open file objects
typedef struct file_table{
  open_file open_files[FTABLE_ENTRIES];
} file_table;

void file_table_init(file_table* file_table){
  for(int i = 0; i < FTABLE_ENTRIES; i++){
    open_file_clear(&file_table->open_files[i]);
  }
}

RSTATUS file_table_add(file_table* file_table, open_file* fileobj){
  for(int i = 0; i < FTABLE_ENTRIES; i++){
    if(open_file_not_present(&file_table->open_files[i]) == 0){
      file_table->open_files[i] = *fileobj;
      return 0;
    }
  }
  return -1;

}

RSTATUS file_table_remove(file_table* file_table, int index){
  if(open_file_not_present(&file_table->open_files[index]) == 0){
    return 0;
  }
  open_file_clear(&file_table->open_files[index]);
  return 1;
}

#endif
