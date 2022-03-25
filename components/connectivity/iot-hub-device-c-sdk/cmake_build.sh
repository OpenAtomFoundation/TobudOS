rm -rf build
git submodule init
git submodule update
mkdir -p build
cd build
cmake .. 
make all -j16
