#!/bin/bash

rm -r build
mkdir build
mkdir build/psp
mkdir build/desktop

cp Assets build/psp/ -r
cp Assets build/desktop/ -r
cp Skin build/psp/ -r
cp Skin build/desktop/ -r
cp Songs build/psp/ -r
cp Songs build/desktop/ -r

cd build/psp
PSPPREFIX=$(psp-config -d)/bin
$PSPPREFIX/psp-cmake ../..
make -j $(nproc)

cd ../desktop
cmake ../..
make -j $(nproc)

cd ../..
