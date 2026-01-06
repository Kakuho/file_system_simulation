set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRCS 
  main.c
)

add_compile_options(-g -O0 -Wall -Wextra)
target_sources(file_system_simulation PRIVATE ${SRCS})
