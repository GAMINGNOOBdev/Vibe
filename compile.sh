#!/bin/bash
clear
make clean
bear -- make -j $(nproc)
