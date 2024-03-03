# !/bin/bash

cmake -B ./build -DCPACK_OUTPUT_FILE_PREFIX=`pwd`/dest
cd build
make
# cpack
./common_util_demo
