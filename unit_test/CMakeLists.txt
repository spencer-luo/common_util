message(STATUS "--------- begin cmake unit_test ---------")

cmake_minimum_required(VERSION 3.14)
project(common_util_unit_test LANGUAGES CXX)

# 与主库保持 C++ 标准一致：
# - 主项目以 CXX_STANDARD_11=ON 配置时使用 C++11；GoogleTest v1.12.x 是
#   最后一个仍支持 C++11 的发布版本。
# - 否则不主动设置 CMAKE_CXX_STANDARD，沿用编译器默认（通常 C++17），
#   保持库与测试代码 __cplusplus 一致，避免 #if __cplusplus 分支不一致
#   引发的链接错误。
if(CXX_STANDARD_11)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
set(CMAKE_CXX_EXTENSIONS OFF)

# 解决 Windows 下 GoogleTest 的 CRT 链接冲突
# Fix runtime mismatch between gtest and the test binary on Windows
if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

# 通过 FetchContent 引入 GoogleTest，避免依赖系统安装。
# Fetch GoogleTest via FetchContent so the unit tests work without a system-wide install.
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    # release-1.12.1 是最后一个支持 C++11 的 GoogleTest 版本，
    # 与 common_util(CXX_STANDARD_11=ON) 兼容。
    GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# 测试用源码：unit_test 目录下的所有 test_*.cpp（包含 test_main.cpp）
file(GLOB UNIT_TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp
)

add_executable(${PROJECT_NAME} ${UNIT_TEST_SRC})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        common_util
        GTest::gtest
)

if(NOT WIN32)
    # 线程相关测试需要链接 pthread，dlloader 测试需要链接 ${CMAKE_DL_LIBS}
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
endif()

# 把每个 TEST 注册到 ctest，便于通过 ctest 单独运行
gtest_discover_tests(${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    PROPERTIES TIMEOUT 120
)

message(STATUS "--------- end cmake unit_test ---------")
