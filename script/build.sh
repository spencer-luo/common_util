#! /bin/bash

usage()
{
    echo "arguments error, usages:"
    echo "$0 build [release|rls|debug|dbg]"
    echo "$0 run"
    echo "$0 clean"
    echo "$0 pack"
    echo "$0 test [filter]      build and run unit tests, filter is an optional regex of test names"
    echo "$0 test clean         remove the unit test build directory"
    echo ""
    echo "environment variables:"
    echo "  UT_CXX11=ON         build unit tests with C++11, the same standard as the release build"
    exit 0
}

# 并行编译的任务数，getconf 在 Linux/macOS 上都可用
jobs_num=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)

# 检查参数是否正确
if [ $# -lt 1 ]; then
    usage
fi

# 执行脚本命令
if [ $1 == "build" ]; then
    if [[ $2 == "release" || $2 == "rls" ]]; then
        echo $2
        build_type="Release"
    elif [[ $2 == "debug" || $2 == "dbg" ]]; then
        echo $2
        build_type="Debug"
    else
        usage
    fi
    echo "Building..."
    # 仅 release 包把构建结果拷到 publish/；debug 保持默认 OFF，避免往 /bin 一类路径误拷
    release_binary=OFF
    if [ "${build_type}" = "Release" ]; then
        release_binary=ON
    fi
    cmake -B ./build -DCMAKE_BUILD_TYPE:STRING=${build_type} -DBUILD_DEMO=ON -DCXX_STANDARD_11=ON -DRELEASE_BINARY_LIBRARY=${release_binary} || exit 1
    cmake --build ./build -j${jobs_num} || exit 1
    echo "Build Done."
elif [ $1 == "pack" ]; then
    echo "Pack package..."
    cd build
    cpack
    echo "pack package Done."
elif [ $1 == "run" ]; then
    # echo "Running..."
    executable_filepath=$(cat ./build/executable_filepath.data)
    echo ${executable_filepath}
    ${executable_filepath}
elif [ $1 == "clean" ]; then
    rm -rf ./build ./publish ./build_ut ./build_ut11
    echo "./build ./publish ./build_ut ./build_ut11 were Cleaned."
elif [ $1 == "test" ]; then
    # C++11 与默认标准使用各自独立的构建目录，避免来回切换时反复全量重编
    ut_dir="build_ut"
    cmake_args="-DBUILD_UNIT_TEST=ON"
    if [ "${UT_CXX11}" = "ON" ]; then
        ut_dir="build_ut11"
        cmake_args="${cmake_args} -DCXX_STANDARD_11=ON"
    fi

    if [ "$2" = "clean" ]; then
        rm -rf ./build_ut ./build_ut11
        echo "./build_ut and ./build_ut11 Cleaned."
        exit 0
    fi

    echo "Configuring unit tests in ${ut_dir}..."
    cmake -S . -B ${ut_dir} ${cmake_args} || exit 1
    echo "Building unit tests..."
    cmake --build ${ut_dir} -j${jobs_num} || exit 1

    # ctest 3.20 起支持 --no-tests=error。没有它时，$2 过滤器写错会出现
    # "No tests were found" 但退出码仍为 0 的假绿。
    no_tests_opt=""
    if ctest --help 2>/dev/null | grep -q -- '--no-tests'; then
        no_tests_opt="--no-tests=error"
    fi

    # 构建失败会在上面直接退出，不会拿旧的二进制跑测试
    echo "Running unit tests..."
    if [ -n "$2" ]; then
        ctest --test-dir ${ut_dir} --output-on-failure -j${jobs_num} ${no_tests_opt} -R "$2"
    else
        ctest --test-dir ${ut_dir} --output-on-failure -j${jobs_num} ${no_tests_opt}
    fi
    # 把 ctest 的退出码作为脚本的退出码，方便 CI 判断
    exit $?
else
    usage
fi
