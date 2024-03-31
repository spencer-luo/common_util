# !/bin/bash
echo $1
if [ $1 == "build" ]; then
    echo "Building..."
    cmake -B ./build -DCPACK_OUTPUT_FILE_PREFIX=`pwd`/dest/pkg
    cd build
    make
    cd ..
    # copy libcommon_util.lib
    if [ ! -d ./dest/common_util/lib/ ]; then
        mkdir -p ./dest/common_util/lib/
    fi
    cp ./build/src/common_util/libcommon_util.a ./dest/common_util/lib/
    # copy header files
    if [ ! -d ./dest/common_util/include/ ]; then
        mkdir -p ./dest/common_util/include/
    fi
    cp ./src/common_util/*.h ./dest/common_util/include/
    # copy demo
    cp build/src/usage_demo/common_util_demo ./dest/
    echo "Build Done."
elif [ $1 == "pack" ]; then
    echo "Running..."
    cd build
    cpack
    echo "pack package Done."
elif [ $1 == "run" ]; then
    echo "Running..."
    cd dest
    ./common_util_demo
elif [ $1 == "clean" ]; then
    rm -rf ./build
    echo "./build Cleaned."
else
    echo "Invalid argument."
fi

# cmake -B ./build -DCPACK_OUTPUT_FILE_PREFIX=`pwd`/dest
# cd build
# make
# # cpack
# ./common_util_demo
