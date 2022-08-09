# How to run the demo in linux

## step1
make sure your develop environment.
+ `cmake` and version greater than 3.8.2
+ `gcc` `gdb` `make` is installed

## step2
make `build` directory and compile in `build`

```bash
mkdir build && cd build
cmake ..
make
```

## step3
run program !!

```bash
# in build directory
./hello_world
```

## other
you can copy this demo to other path, but if you want do it,
you need modify `CMakeLists.txt`. find line

```cmake
set(TINY_ROOT ../../../)
```

and modify `path-to-tinyos`
```cmake
set(TINY_ROOT path-to-tinyos)
```
