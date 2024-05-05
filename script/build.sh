# !/bin/bash
echo $1
if [ $1 == "build" ]; then
    echo "Building..."
    cmake -B ./build
    cd build
    make
    echo "Build Done."
elif [ $1 == "pack" ]; then
    echo "Running..."
    cd build
    cpack
    echo "pack package Done."
elif [ $1 == "run" ]; then
    echo "Running..."
    # cd dest
    # ./common_util_demo
    executable_filepath=$(cat ./build/executable_filepath.data)
    echo ${executable_filepath}
    ${executable_filepath}
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
