#! /bin/bash

usage()
{
    echo "arguments error, usages:"
    echo "$0 build [release|rls|debug|dbg]"
    echo "$0 run"
    echo "$0 clean"
    echo "$0 pack"
    exit 0
}

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
    # cmake -B ./build -DCPACK_OUTPUT_FILE_PREFIX=`pwd`/dest
    cmake -B ./build -DCMAKE_BUILD_TYPE:STRING=${build_type}
    cd build
    make
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
    rm -rf ./build
    echo "./build Cleaned."
else
    echo "Invalid argument."
fi