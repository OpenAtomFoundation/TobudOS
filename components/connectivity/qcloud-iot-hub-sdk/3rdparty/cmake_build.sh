#! /bin/bash

if [ "$#" -eq "0" ]; then
    rm -rf output
    rm -rf build
    echo "Build all (SDK libs and samples)"
    mkdir -p build
    cd build
    cmake .. 
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
elif [ "$#" -eq "1"  -a $1 == "code-check" ]; then
# code-check requires clang-tidy and cpplint
# sudo apt install clang-tidy
# pip install cpplint
    echo "run code check" 
    rm -rf build
    mkdir -p build
    cd build
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
    echo "clang-tidy check begin"
    clang_extra_arg="-extra-arg=-I/usr/lib/gcc/x86_64-linux-gnu/7/include -extra-arg=-I/usr/lib/gcc/x86_64-linux-gnu/7/include-fixed"
    clang_check_rules="-checks=-clang-analyzer-osx*,-clang-analyzer-security.insecureAPI.strcpy,-clang-diagnostic-missing-braces,-clang-diagnostic-varargs"
    ../tools/run-clang-tidy ${clang_extra_arg} ${clang_check_rules} >../code-check-clang-tidy.txt
    echo "clang-tidy check end"
    echo "cpplint check begin"
    cd ..
    # find ./samples/ ./include/ ./sdk_src/ ./platform/ -name *.c -o -name *.h | xargs clang-format -i
    cpplint_rules="--filter=-whitespace/line_length,-readability/casting,-build/include_subdir,-whitespace/braces,-whitespace/comments,-whitespace/blank_line,-whitespace/parens,-whitespace/semicolon,-whitespace/end_of_line,-whitespace/ending_newline,-whitespace/comma,-build/header_guard,-runtime/int,-runtime/printf,-readability/todo,-build/include_order,-build/include_what_you_use"
    cpplint_coverage="include/ samples/ platform/ sdk_src/"
    cpplint ${cpplint_rules} --recursive ${cpplint_coverage} &>./code-check-cpplint.txt
    echo "cpplint check end"
    exit
else
	echo "Usage: "$0"  <null for all/samples/clean/code-check>"
	exit
fi
