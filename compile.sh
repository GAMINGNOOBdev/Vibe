#!/bin/bash

rm -r build
mkdir build
mkdir build/psp
mkdir build/desktop

cd build/psp
PSPPREFIX=$(psp-config -d)/bin
$PSPPREFIX/psp-cmake ../.. -D CMAKE_BUILD_TYPE=Debug
make -j $(nproc)

cd ../desktop
cmake ../.. -D CMAKE_BUILD_TYPE=Debug
make -j $(nproc)

cd ../..
