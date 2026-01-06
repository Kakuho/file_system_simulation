#include "ramdisk.h"

void sample_ramdisk_10(){
  ramdisk* disk = ramdisk_create(5, 10);
  if(disk == NULL){
    return;
  }
  ramdisk_print_properties(disk);
  ramdisk_print(disk);
  char write[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  if(ramdisk_write(disk, write, 1) != 0){
    ramdisk_destroy(disk);
    return;
  }
  printf("Successful Write\n");
  ramdisk_print_properties(disk);
  ramdisk_print(disk);
  ramdisk_destroy(disk);
}

int main(){
  sample_ramdisk_10();
}
