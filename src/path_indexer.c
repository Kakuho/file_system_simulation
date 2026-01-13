#include "path_indexer.h"

static void print_component(component_indicies* indicies, const char* string){
  int32_t chidx = indicies->start;
  do{
    printf("%c", string[chidx]);
    chidx++;
  }
  while(chidx <= indicies->end); 
}

RSTATUS path_indexer_parse(path_indexer* path_indexer, const char* path){
  //  for starting, you have 2 special characters, or you can use the alphanumeric
  //    ./fkadj/fkadsjf/ - start is index 0 and end is index 1, similiar to an alphanumeric start
  //    /                - start and end is at index 0
  //
  //  for ending you can either use / or not
  //  /hiya/baka          - start = 6 end = 9
  //  /hiya/baka.txt      - start = 6 end = 13
  //  /hiya/baka/         - start = 6 end = 10
  size_t index = 0;
  size_t path_length = strlen(path);
  debug_log("path length: %lu\n", path_length);
  path_indexer->ncomponents = 0;
  path_indexer->path = path;

  if(path[0] == '/'){
    debug_log("Absolute Path");
    path_indexer->entries[0].start = 0;
    path_indexer->entries[0].end = 0;
    path_indexer->ncomponents++;
    index++;
  }
  while(1){
    int32_t comp_start;
    if(path_indexer->ncomponents == 0){
      comp_start = 0;
    }
    else{
      comp_start = path_indexer->entries[index-1].end + 1;
    }
    //printf("next start: %d \n", comp_start);
    // parse the next component
    int32_t comp_indexer = comp_start;
    while(path[comp_indexer] != '/' && comp_indexer < path_length){
      //printf("comp indexer: %d \n", comp_indexer);
      comp_indexer++;
    }
    //printf("next end: %d \n", comp_indexer);
    if(comp_indexer == path_length){
      //printf("comp indexer == path length\n");
      comp_indexer--;
      path_indexer->entries[index].start = comp_start;
      path_indexer->entries[index].end = comp_indexer;
      path_indexer->ncomponents++;
      break;
    }
    else{
      path_indexer->entries[index].start = comp_start;
      path_indexer->entries[index].end = comp_indexer;
      path_indexer->ncomponents++;
      index++;
    }
    debug_log("component start: %d\n", path_indexer->entries[index-1].start);
    debug_log("component end: %d\n", path_indexer->entries[index-1].end);
  }
  return 0;
}

RSTATUS path_indexer_read_component(path_indexer* path_indexer, unsigned index, char* buffer, int32_t* length){
  assert(index < path_indexer->ncomponents);
  int32_t start = path_indexer->entries[index].start;
  int32_t end = path_indexer->entries[index].end;
  *length = end - start;
  if(*length == 0){
    memcpy(buffer, &path_indexer->path[start], 1);
    *length = 1;
  }
  else if(path_indexer->path[end] == '/'){
    memcpy(buffer, &path_indexer->path[start], end - start);
  }
  else{
    memcpy(buffer, &path_indexer->path[start], end - start + 1);
    *length += 1;
  }
  debug_log("length: %d\n", *length);
  return 0;
}

RSTATUS path_indexer_print(path_indexer* path_indexer){
  uint32_t compidx= 0;
  char comp_buffer[20];
  while(compidx != path_indexer->ncomponents){
    printf("Component: %u\n", compidx);
    component_indicies* indicies = &path_indexer->entries[compidx];
    print_component(indicies, path_indexer->path);
    printf("\n");
    compidx++;
  }
  return 0;
}

RSTATUS path_indexer_print_indicies(path_indexer* path_indexer){
  uint32_t compidx= 0;
  while(compidx != path_indexer->ncomponents){
    printf("Component %u indicies:\n", compidx);
    component_indicies* indicies = &path_indexer->entries[compidx];
    printf("start: %d\nend: %d\n", indicies->start, indicies->end);
    compidx++;
  }
  return 0;
}
