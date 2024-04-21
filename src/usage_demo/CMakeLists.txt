message(STATUS "--------- begin cmake usage_demo ---------")

# cmake工具的最低版本号
# cmake version
cmake_minimum_required(VERSION 3.28.3)
# 项目名称
# project name
project(common_util_demo LANGUAGES CXX)
message("PROJECT_NAME:" ${PROJECT_NAME})

# C/C++标准的版本
# C/C++ standard: C99/C++11
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# 添加头文件的包含目录
# include directories
message("CMAKE_SOURCE_DIR:" ${CMAKE_SOURCE_DIR})
# todo
include_directories(${CMAKE_SOURCE_DIR}/src)

# 构建可执行文件
# build library
# add_library(common_util STATIC ${COMMON_UTIL_SRC})
add_executable(${PROJECT_NAME} main.cpp)
# 添加链接库
target_link_libraries(${PROJECT_NAME} common_util)

set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/dest/)
# 拷贝编译结果文件
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    # 创建文件夹: dest/
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
    # 拷贝编译结果文件: libcommon_util_demo
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${OUTPUT_DIR}
)

message(STATUS "--------- end cmake usage_demo ---------")
