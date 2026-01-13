#ifndef PATH_INDEXER_H
#define PATH_INDEXER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "types.h"
#include "debug/debug_log.h"

typedef struct component_indicies{
  // invalid start and ends are marked with -1
  int32_t start;   
  int32_t end;       // inclusive (with the /)
} component_indicies;

typedef struct path_indexer{
  const char* path;
  struct component_indicies entries[6];
  uint32_t ncomponents;
} path_indexer;     

RSTATUS path_indexer_parse(path_indexer* path_indexer, const char* path);
RSTATUS path_indexer_read_component(path_indexer* path_indexer, unsigned index, char* buffer, int32_t* length);

RSTATUS path_indexer_print(path_indexer* path_indexer);
RSTATUS path_indexer_print_indicies(path_indexer* path_indexer);

#endif
