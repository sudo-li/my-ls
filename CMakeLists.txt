cmake_minimum_required(VERSION 3.16)
project(my_ls C)

set(CMAKE_C_STANDARD 99)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(my_ls my_ls.c my_ls.h)
