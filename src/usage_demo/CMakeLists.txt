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
include_directories(${CMAKE_SOURCE_DIR}/src/common_util)

# 构建可执行文件
# build library
# add_library(common_util STATIC ${COMMON_UTIL_SRC})
add_executable(${PROJECT_NAME} main.cpp)
# 添加链接库
target_link_libraries(${PROJECT_NAME} common_util)

# 拷贝编译结果文件
# copy compiled files
set(publish_dir ${CMAKE_SOURCE_DIR}/publish)
set(output_dir ${publish_dir}/${PROJECT_NAME})
message("CMAKE_COMMAND:" ${CMAKE_COMMAND})
message("output_dir:" ${output_dir})

# 获取构建目标的CPU架构
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(library_dir ${CMAKE_CURRENT_BINARY_DIR}/Release)
    else()
        set(library_dir ${CMAKE_CURRENT_BINARY_DIR}/Debug)
    endif()
else()
    set(library_dir ${CMAKE_CURRENT_BINARY_DIR})
endif()
message("library_dir:" ${library_dir})

# 可执行文件的名称
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(executable_name ${PROJECT_NAME}.exe)
else()
    set(executable_name ${PROJECT_NAME})
endif()
message("executable_name:" ${executable_name})

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("start to copy target files")
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        # 创建文件夹: dest/
        COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir}/${CMAKE_SYSTEM_NAME}/${target_arch}/
        # 拷贝编译结果文件: common_util_demo
        COMMAND ${CMAKE_COMMAND} -E copy ${library_dir}/${executable_name} ${output_dir}/${CMAKE_SYSTEM_NAME}/${target_arch}
    )
endif()

# 记录可执行文件的路径到./build/executable_filepath.data
message("write executable_filepath.data: " ${library_dir}/${executable_name})
file(WRITE
    ${CMAKE_BINARY_DIR}/executable_filepath.data
    ${library_dir}/${executable_name}
)

# publish/common_demo/
# publish/common_demo/windows/x32/common_demo.exe
# publish/common_demo/windows/x64/common_demo.exe
# publish/common_demo/Darwin/x86_64/common_demo

message(STATUS "--------- end cmake usage_demo ---------")
