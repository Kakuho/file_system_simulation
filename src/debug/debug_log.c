#include "debug_log.h"

void debug_log(const char* format, ...){
  if(DEBUG){
    va_list arglist;
    printf("DEBUG: ");
    va_start(arglist, format);
    vprintf(format, arglist);
    va_end(arglist);
  }
}
