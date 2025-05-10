message(STATUS "--------- begin cmake common_util ---------")

# cmake工具的最低版本
# cmake  minimum version
cmake_minimum_required(VERSION 3.28.3)

# 项目名称
# project name
project(common_util VERSION 1.2.0 LANGUAGES CXX)
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
message(DEBUG "COMMON_UTIL_SRC:" ${COMMON_UTIL_SRC})

# 编译成静态库
# build library
add_library(${PROJECT_NAME} STATIC ${COMMON_UTIL_SRC})

# 添加头文件的包含目录
# include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  # 构建时使用源码目录的 include
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}># 安装后使用安装目录的 include
)

# 如果是windows，设置 管理员权限 和 UAC绕过UI保护
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/level='requireAdministrator' " )
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/level='requireAdministrator' /uiAccess='true'" )
endif()

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

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(library_name ${PROJECT_NAME}.lib)
else()
    set(library_name lib${PROJECT_NAME}.a)
endif()
message("library_name:" ${library_name})

message("CMAKE_BUILD_TYPE:" ${CMAKE_BUILD_TYPE})
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("start to copy target files")

    # 在编译完(POST_BUILD) 构建目标(TARGET) common_util后，执行的命令
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        # 创建文件夹: dest/common_util/lib
        COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir}/lib/${CMAKE_SYSTEM_NAME}/${target_arch}
        # 拷贝编译结果文件: libcommon_util.a
        COMMAND ${CMAKE_COMMAND} -E copy ${library_dir}/${library_name} ${output_dir}/lib/${CMAKE_SYSTEM_NAME}/${target_arch}
        # 创建文件夹: dest/common_util/include
        COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir}/include/
        # 拷贝头文件: src/common_util/*.h
        # COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${output_dir}/include/
        COMMAND ${CMAKE_COMMAND} -E copy ${COMMON_UTIL_HEADER} ${output_dir}/include/
    )
elseif()
    message("current build is debug")
endif()
# publish/common_util/
# publish/common_util/include/
# publish/common_util/windows/x32/common_util.lib
# publish/common_util/windows/x64/common_util.lib
# publish/common_util/Darwin/x86_64/common_util.lib

message(STATUS "--------- end cmake common_util ---------")
