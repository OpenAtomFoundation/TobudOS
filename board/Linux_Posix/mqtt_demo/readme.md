# How to run the demo in linux

## step1
make sure your develop environment.
+ `cmake` and version greater than 3.8.2
+ `gcc` `gdb` `make` is installed

## step2
generate `mqtt_config.h` file to replace `./inc/mqtt_config.h`

cd `tiny/tools/` directory, run python script
```bash
python3 mqtt_config_gen.py
```

then input your server configuration, generate `mqtt_config.h` file
copy to `./inc/` replace old file

## step3
make `build` directory and compile in `build`

```bash
mkdir build && cd build
cmake ..
make
```

## step4
run program !!

```bash
# in build directory
./mqtt_demo
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
