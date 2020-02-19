#!/bin/bash

mkdir -p build build/bin build/lib
cd build
cmake ..
make
