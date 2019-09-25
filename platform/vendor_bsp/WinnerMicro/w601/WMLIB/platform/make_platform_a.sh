#!/bin/bash
set -e

export SDK_PATH=$(dirname $(pwd))

echo "make_lib.sh version 20150924"
echo ""

for dir in common drivers sys; do
    cd $dir
	make clean
	make COMPILE=gcc
	echo "move lib$dir.lib to lib folder"
	cp .output/w600/lib/lib$dir.a ../../lib/lib$dir.a
    cd ..
done;