message(STATUS "--------- begin cmake ---------")

# cmake教程：https://www.zhihu.com/column/c_1369781372333240320


# cmake version
cmake_minimum_required(VERSION 3.28.3)
# project name
project(common_util VERSION 1.0.0 LANGUAGES CXX)
# C/C++ standard: C99/C++11
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

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

# message("CMAKE_CURRENT_SOURCE_DIR:" ${CMAKE_CURRENT_SOURCE_DIR})
# message("CMAKE_SOURCE_DIR:" ${CMAKE_SOURCE_DIR})

# add sub moudle for common_util
add_subdirectory(src/common_util)
add_subdirectory(src/usage_demo)

# pack and install
# set pack content
install(TARGETS common_util common_util_demo
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    # ARCHIVE DESTINATION pack
)
# install headers for common_util
file(GLOB COMMON_UTIL_HEADERS src/common_util/*.h)
install(FILES ${COMMON_UTIL_HEADERS} DESTINATION include)
# set pack config
include(CPack)
set(CPACK_GENERATOR "ZIP") # pack type
set(CPACK_PACKAGE_NAME "common_util")
set(CPACK_INSTALL_PREFIX "") # path for pack inner
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})


message(STATUS "--------- end cmake ---------")