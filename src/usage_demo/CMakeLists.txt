message(STATUS "--------- begin cmake usage_demo ---------")

# cmake version
cmake_minimum_required(VERSION 3.28.3)
# project name
project(common_util_demo LANGUAGES CXX)
message("PROJECT_NAME:" ${PROJECT_NAME})
# C/C++ standard: C99/C++11
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# include directories
message("CMAKE_SOURCE_DIR:" ${CMAKE_SOURCE_DIR})
# todo
include_directories(${CMAKE_SOURCE_DIR}/src)

# build library
# add_library(common_util STATIC ${COMMON_UTIL_SRC})
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(common_util_demo common_util)

# message("CMAKE_CURRENT_BINARY_DIR:" ${CMAKE_CURRENT_BINARY_DIR})
# file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/dest/)

message(STATUS "--------- end cmake common_util ---------")
