#!/bin/bash

mkdir -p build build/bin build/lib
cd build

if [ " $1" == " " ]; then
    compiler="gcc"
elif [ $1 == "--help" -o $1 == "-help" -o  $1 == "--h" -o $1 == "-h" ]; then
    echo "usage: $0 [compiler] [compiler path]"
    echo "  compiler: specify the compiler you are using, default: gcc"
    echo "  compiler path: specify the compiler path you are using"
    echo "  eg:"
    echo "      ./build.sh"
    echo "      ./build.sh arm-linux-gnueabihf-gcc"
    echo "      ./build.sh /usr/bin/arm-linux-gnueabihf-gcc"
    echo "      ./build.sh arm-linux-gnueabihf-gcc /usr/bin"
    exit
else
    if [ " $2" == " " ]; then
        compiler=$1
    else
        compiler=$2/$1
    fi
fi

path=$(which $compiler)

if [ " $path" == " " ]; then
    echo -e "\033[31mNo $compiler compiler found in the system\033[0m"
    exit
fi

cmake .. "-DCMAKE_C_COMPILER=$path"

make
