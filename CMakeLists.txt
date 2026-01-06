cmake_minimum_required(VERSION 3.1...3.29)

project(file_system_simulation 
          VERSION 1.0
          DESCRIPTION "project for me to learn how to code in C and how to implement file system algorithms"
          LANGUAGES C)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_C_STANDARD 90)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-g)
add_executable(file_system_simulation)
include_directories(src)
add_subdirectory(src)
