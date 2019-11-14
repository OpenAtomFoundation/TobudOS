if [ "$#" -eq "0" ]; then
    rm -rf output
    rm -rf build
    echo "Build all (SDK libs and samples)"
    mkdir -p build
    cd build
    cmake .. 
    #-DSAMPLE_ONLY=OFF ..
    make
    exit
elif [ "$#" -eq "1"  -a $1 == "samples" ]; then
    if [ ! -d "output/release/bin" ]; then
        rm -rf output/release/bin
    elif [ ! -d "output/debug/bin" ]; then
        rm -rf output/debug/bin
    else
        echo "Output folder not found! Please build SDK first"
        exit
    fi
    rm -rf build
    echo "Build samples only"
    mkdir -p build
    cd build
    cmake -DSAMPLE_ONLY=ON ..
    make 
    exit
elif [ "$#" -eq "1"  -a $1 == "clean" ]; then
    echo "Clean all (SDK lib and samples)" 
    rm -rf output
    rm -rf build    
    exit
else
	echo "Usage: "$0"  <null for all/samples/clean>"
	exit
fi
