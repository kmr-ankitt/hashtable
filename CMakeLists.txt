cmake_minimum_required(VERSION 3.10)

project(hashtable C)

set(CMAKE_C_STANDARD 11)

set(SOURCES
    src/main.c
    src/hash_table.c
)

add_executable(hashtable ${SOURCES})