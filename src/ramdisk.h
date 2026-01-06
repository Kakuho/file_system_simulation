#ifndef RAMDISK_H
#define RAMDISK_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "types.h"

typedef struct ramdisk{
  char* base;
  unsigned nblocks;
  unsigned blocksize;
} ramdisk;

ramdisk* ramdisk_create(unsigned nblocks, unsigned blocksize);
void ramdisk_destroy(ramdisk* disk);

void ramdisk_print(ramdisk* disk);
void ramdisk_dump(ramdisk* disk, const char* file_path);
void ramdisk_print_properties(ramdisk* disk);

char* ramdisk_index_to_baddr(ramdisk* disk, unsigned index);

RSTATUS ramdisk_read(ramdisk* disk, char* buffer, unsigned index);
RSTATUS ramdisk_write(ramdisk* disk, char* data, unsigned index);

#endif
