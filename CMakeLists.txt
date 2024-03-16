# cmake version
cmake_minimum_required(VERSION 3.28.3)
# project name
project(common_util VERSION 1.0.0 LANGUAGES CXX)
# C/C++ standard: C99/C++11
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# build library for common_util
file(GLOB_RECURSE COMMON_UTIL_LIB_SRC
    src/common_util/*.cpp
)
add_library(common_util STATIC ${COMMON_UTIL_LIB_SRC})

# message("PROJECT_NAME:" ${PROJECT_NAME})
message("CMAKE_CURRENT_SOURCE_DIR:" ${CMAKE_CURRENT_SOURCE_DIR})

# add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/common_util)

# # print all header files
# file(GLOB_RECURSE ALL_HEADER
#         src/*.h)
# list(JOIN ALL_HEADER "\n" ALL_HEADER_STR)
# message("ALL_HEADER:\n" ${ALL_HEADER_STR})

# build executable for common_util_demo
add_executable(common_util_demo src/main.cpp)
target_link_libraries(common_util_demo common_util)


# install targets to dist
install(TARGETS common_util common_util_demo
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    # ARCHIVE DESTINATION pack
)
# install headers for common_util
file(GLOB_RECURSE COMMON_UTIL_HEADERS src/common_util/*.h)
install(FILES ${COMMON_UTIL_HEADERS} DESTINATION include)

include(CPack)
set(CPACK_GENERATOR "ZIP") # pack type
set(CPACK_PACKAGE_NAME "common_util")
set(CPACK_INSTALL_PREFIX "") # path for pack inner
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
