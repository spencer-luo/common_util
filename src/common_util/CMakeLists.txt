message(STATUS "--------- begin cmake common_util ---------")

# cmake version
cmake_minimum_required(VERSION 3.28.3)
# project name
project(common_util VERSION 1.0.0 LANGUAGES CXX)
message("PROJECT_NAME:" ${PROJECT_NAME})

# C/C++ standard: C99/C++11
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# search source files for .cpp 
message("CMAKE_CURRENT_SOURCE_DIR:" ${CMAKE_CURRENT_SOURCE_DIR})
message("CMAKE_SOURCE_DIR:" ${CMAKE_SOURCE_DIR})
file(GLOB_RECURSE COMMON_UTIL_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
message(DEBUG "COMMON_UTIL_SRC": ${COMMON_UTIL_SRC})

# include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# build library
add_library(common_util STATIC ${COMMON_UTIL_SRC})

# # copy libcommon_util.lib
# message("CMAKE_CURRENT_BINARY_DIR:" ${CMAKE_CURRENT_BINARY_DIR})
# file(COPY ${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.a DESTINATION ${CMAKE_SOURCE_DIR}/dest/${PROJECT_NAME}/lib/)
# # copy header files
# file(GLOB COMMON_UTIL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
# message("COMMON_UTIL_HEADERS:" ${COMMON_UTIL_HEADERS})
# file(COPY ${COMMON_UTIL_HEADERS} DESTINATION ${CMAKE_SOURCE_DIR}/dest/${PROJECT_NAME}/include/)

message(STATUS "--------- end cmake common_util ---------")
