message(STATUS "--------- begin cmake ---------")

# cmake教程：https://www.zhihu.com/column/c_1369781372333240320


# cmake version
cmake_minimum_required(VERSION 3.28.3)
# project name
project(common_util VERSION 1.4.3 LANGUAGES CXX)
# 构建时间
# build time
string(TIMESTAMP BUILD_TIME "%y-%m-%d %H:%M:%S")

# C/C++ standard: C99/C++11
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# 根据version.h.in模板生产version.h头文件
# generate version.h from version.h.in
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/common_util/version.h)

# 获取构建目标的CPU架构
message("CMAKE_SYSTEM_NAME:" ${CMAKE_SYSTEM_NAME})
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    if(CMAKE_CL_64)
        set(target_arch x64)
    else()
        set(target_arch x86)
    endif()
else()
    set(target_arch ${CMAKE_SYSTEM_PROCESSOR})
endif()
message("target_arch:" ${target_arch})

# 查找要编译的.cpp文件
# search source files for .cpp 
# message("CMAKE_CURRENT_SOURCE_DIR:" ${CMAKE_CURRENT_SOURCE_DIR})
# message("CMAKE_SOURCE_DIR:" ${CMAKE_SOURCE_DIR})
file(GLOB_RECURSE COMMON_UTIL_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common_util/*.cpp
)
message(DEBUG "COMMON_UTIL_SRC:" ${COMMON_UTIL_SRC})

# 编译成静态库
# build library
add_library(${PROJECT_NAME} STATIC ${COMMON_UTIL_SRC})

# 添加头文件的包含目录
# include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC  # 当前项目 和 调用方项目 都可见(可以使用)
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  # 构建时使用源码目录的 include
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>        # 安装后使用安装目录的 include
    PRIVATE # 仅 当前项目 可见(可以使用)
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/common_util>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/common_util>
)

# 如果是windows，设置 管理员权限 和 UAC绕过UI保护
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/level='requireAdministrator' " )
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/level='requireAdministrator' /uiAccess='true'" )
endif()

# 拷贝编译结果文件
# copy compiled files
set(publish_dir ${CMAKE_SOURCE_DIR}/publish)
message("publish_dir:" ${publish_dir})

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
        COMMAND ${CMAKE_COMMAND} -E make_directory ${publish_dir}/lib/${CMAKE_SYSTEM_NAME}/${target_arch}
        # 拷贝编译结果文件: libcommon_util.a
        COMMAND ${CMAKE_COMMAND} -E copy ${library_dir}/${library_name} ${publish_dir}/lib/${CMAKE_SYSTEM_NAME}/${target_arch}
        # 创建文件夹: dest/include/common_util
        COMMAND ${CMAKE_COMMAND} -E make_directory ${publish_dir}/include/common_util/
        # 拷贝头文件: include/common_util/*.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/include/common_util/* ${publish_dir}/include/common_util/
    )
elseif()
    message("current build is debug")
endif()

# 默认不编译usage_demo，cmake命令行参数传递BUILD_DEMO=on时才会编译
option(BUILD_DEMO "Need to build usage_demo" OFF)

if (BUILD_DEMO)
    # add sub moudle for usage_demo
    add_subdirectory(src/usage_demo)
endif()

# # 打包安装
# # pack and install
# # set pack content
# set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_SOURCE_DIR}/publish/pkg)
# message("CPACK_OUTPUT_FILE_PREFIX:" ${CPACK_OUTPUT_FILE_PREFIX})

# install(TARGETS common_util common_util_demo
#     RUNTIME DESTINATION bin
#     LIBRARY DESTINATION lib
#     # ARCHIVE DESTINATION pack
# )
# # install headers for common_util
# install(FILES ${COMMON_UTIL_HEADERS} DESTINATION include)
# # set pack config
# include(CPack)
# set(CPACK_GENERATOR "ZIP") # pack type
# set(CPACK_PACKAGE_NAME "common_util")
# set(CPACK_INSTALL_PREFIX "") # path for pack inner
# set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})


message(STATUS "--------- end cmake ---------")