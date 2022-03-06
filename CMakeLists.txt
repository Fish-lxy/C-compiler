cmake_minimum_required(VERSION 3.20)
project(C_compiler C)

set(CMAKE_C_STANDARD 99)

add_executable(C_compiler main.c vm.c vm.h compiler.c compiler.h)
