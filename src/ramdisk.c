#include "ramdisk.h"

ramdisk* ramdisk_create(unsigned nblocks, unsigned blocksize){
  ramdisk* disk = (ramdisk*)malloc(sizeof(ramdisk));
  if(disk == NULL){
    return NULL;
  }
  char* buffer = (char*)malloc(sizeof(char) * nblocks * blocksize);
  if(buffer == NULL){
    free(disk);
    return NULL;
  }
  disk->base = buffer;
  disk->nblocks = nblocks;
  disk->blocksize = blocksize;
  memset(disk->base, 0, disk->nblocks * disk->blocksize);
  return disk;
}

void ramdisk_destroy(ramdisk* disk){
  free(disk->base);
  free(disk);
}

void ramdisk_print(ramdisk* disk){
  printf("max index: %d\n", disk->nblocks * disk->blocksize);
  int i;
  for(i = 0; i < (disk->nblocks * disk->blocksize); i++){
    if(i == 0){
      printf("Block %d:\n", i/disk->blocksize);
    }
    else if(i % disk->blocksize == 0){
      printf("\n\nBlock %d:\n", i/disk->blocksize);
    }
    printf("%x ", disk->base[i] & 0xFF);
    if(i != 0 && (i+1) % 64 == 0){
      printf("\n");
    }
  }
  printf("\n");
}

void ramdisk_print_block(ramdisk* disk, unsigned block){
  int i;
  for(i = 0; i < disk->blocksize; i++){
    if(i == 0){
      printf("Block %d:\n", block);
    }
    printf("%x ", disk->base[(disk->blocksize * block) + i] & 0xFF);
    if(i != 0 && (i+1) % 64 == 0){
      printf("\n");
    }
  }
  printf("\n");
}

void ramdisk_dump(ramdisk* disk, const char* file_path){
  FILE* file = fopen(file_path, "w");
  if(file == NULL){
    return;
  }
  fprintf(file, "max index: %d\n", disk->nblocks * disk->blocksize);
  int i;
  for(i = 0; i < (disk->nblocks * disk->blocksize); i++){
    if(i == 0){
      fprintf(file, "Block %d:\n", i/disk->blocksize);
    }
    else if(i % disk->blocksize == 0){
      fprintf(file, "\n\nBlock %d:\n", i/disk->blocksize);
    }
    fprintf(file, "%x ", disk->base[i] & 0xFF);
    if(i != 0 && (i+1) % 64 == 0){
      fprintf(file, "\n");
    }

  }
  fprintf(file, "\n");
}

void ramdisk_print_properties(ramdisk* disk){
  printf("Printing Ram Disk Properties...\n");
  printf("Base: %x\n", (uintptr_t)disk->base);
  printf("Blocks: %u\n", disk->nblocks);
  printf("Block Size: %u\n", disk->blocksize);
}

char* ramdisk_index_to_baddr(ramdisk* disk, unsigned index){
  uintptr_t base = (uintptr_t)(disk->base);
  char* addr = (char*)(base + (index * disk->blocksize));
  debug_log("base: %x\n", base);
  debug_log("addr: %x\n", addr);
  return addr;
}

RSTATUS ramdisk_read(ramdisk* disk, char* buffer, unsigned index){
  if(index > disk->nblocks - 1){
    return -1;
  }
  char* addr = ramdisk_index_to_baddr(disk, index);
  memcpy(buffer, addr, disk->blocksize);
  return 0;
}

RSTATUS ramdisk_write(ramdisk* disk, char* data, unsigned index){
  if(index > (disk->nblocks - 1)){
    return -1;
  }
  char* addr = ramdisk_index_to_baddr(disk, index);
  memcpy(addr, data, disk->blocksize);
  return 0;
}
