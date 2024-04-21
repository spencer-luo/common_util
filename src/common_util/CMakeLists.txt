message(STATUS "--------- begin cmake common_util ---------")

# cmake工具的最低版本
# cmake  minimum version
cmake_minimum_required(VERSION 3.28.3)

# 项目名称
# project name
project(common_util VERSION 1.0.0 LANGUAGES CXX)
# 构建时间
# build time
string(TIMESTAMP BUILD_TIME "%y-%m-%d %H:%M:%S")

message("PROJECT_NAME:" ${PROJECT_NAME})
message("BUILD_TIME:" ${BUILD_TIME})

# 根军version.h.in模板生产version.h头文件
# generate version.h from version.h.in
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

# 设置C/C++标准的版本
# C/C++ standard: C99/C++11
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# 查找要编译的.cpp文件
# search source files for .cpp 
message("CMAKE_CURRENT_SOURCE_DIR:" ${CMAKE_CURRENT_SOURCE_DIR})
message("CMAKE_SOURCE_DIR:" ${CMAKE_SOURCE_DIR})
file(GLOB_RECURSE COMMON_UTIL_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
message(DEBUG "COMMON_UTIL_SRC": ${COMMON_UTIL_SRC})

# 添加头文件的包含目录
# include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 斌阿姨静态库
# build library
add_library(${PROJECT_NAME} STATIC ${COMMON_UTIL_SRC})

# 拷贝编译结果文件
# copy compiled files
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/dest/${PROJECT_NAME})
message("CMAKE_COMMAND:" ${CMAKE_COMMAND})
message("OUTPUT_DIR:" ${OUTPUT_DIR})
# 在编译完(POST_BUILD) 构建目标(TARGET) common_util后，执行的命令
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    # 创建文件夹: dest/common_util/lib
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}/lib/
    # 拷贝编译结果文件: libcommon_util.a
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.a ${OUTPUT_DIR}/lib/
    # 创建文件夹: dest/common_util/include
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}/include/
    # 拷贝头文件: src/common_util/*.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${OUTPUT_DIR}/include/
)

message(STATUS "--------- end cmake common_util ---------")
