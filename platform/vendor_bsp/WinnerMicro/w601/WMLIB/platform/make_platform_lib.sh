#!/bin/bash
set -e

echo "make_lib.sh"
echo ""

for dir in common drivers sys; do
    cd $dir
	make clean
	make COMPILE=armcc
	echo "move lib$dir.lib to lib folder"
	cp .output/w600/lib/lib$dir.lib ../../lib/lib$dir.lib
    cd ..
done;