#! /bin/bash

script_dir=$(dirname $0)
echo ${script_dir}

doxygen ${script_dir}/doxygen.cfg
