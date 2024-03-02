cmake_minimum_required(VERSION 3.28.3)

project(common_util VERSION 1.0.0 LANGUAGES CXX)

# print all header files
file(GLOB_RECURSE ALL_HEADER
        src/*.h)
list(JOIN ALL_HEADER "\n" ALL_HEADER_STR)
message("ALL_HEADER:\n" ${ALL_HEADER_STR})

add_executable(common_util_demo src/main.cpp)
