#!/bin/bash

mkdir -p build
mkdir -p build/psp
mkdir -p build/desktop

cd build

if [ $1 = 'psp' ] ; then
    cd psp
    echo "=== Building psp ==="

    PSPPREFIX=$(psp-config -d)/bin
    $PSPPREFIX/psp-cmake ../.. -D CMAKE_BUILD_TYPE=Debug
    time make -j $(nproc)

    echo "=== End of build ==="
    cd ../..
else
    cd desktop
    echo "=== Building desktop ==="

    cmake ../.. -D CMAKE_BUILD_TYPE=Debug
    time make

    echo "=== End of build ==="
    cd ../..
fi
