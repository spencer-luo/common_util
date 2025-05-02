
#! /bin/bash

cppcheck --std=c++11 --enable=all ./src --output-file=./cppcheck_report.txt
