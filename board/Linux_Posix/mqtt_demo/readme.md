# How to run the demo in linux

## step1
make sure your develop environment.
+ `cmake` and version greater than 3.8.2
+ `gcc` `gdb` `make` is installed

## step2
configure your server and add a transmit rule

reference [tencent cloud IoT guide?](../../../doc/8.TencentOS-tiny对接腾讯云IoTHub开发指南.md)

additional thing, add a rule for send a message to the demo
1. select filter topic is the ${demo device}/event and 
SELECT * FROM ${demo device}/event
2. select action type is Republish and topic is the ${demo device}/control

the page like this
_____________________________________________
|规则引擎
|--------------------------------------------
|基本信息
|规则名称  loop
|规则描述  未填写
|--------------------------------------------
|筛选数据
|字段  *
|Topic  ${demo device}/event
|条件
|当前SQL语句是：
|SELECT * ${demo device}/event
|--------------------------------------------
|行为操作
|行为类型  数据转发到另一个Topic ( Republish )
|Topic ${demo device}/control
|  ...
_____________________________________________


## step3
generate `mqtt_config.h` file to replace `./inc/mqtt_config.h`

cd `tiny/tools/` directory, run python script
```bash
python3 mqtt_config_gen.py
```

then input your server configuration, generate `mqtt_config.h` file
copy to `./inc/` replace old file

## step4
make `build` directory and compile in `build`

```bash
mkdir build && cd build
cmake ..
make
```

## step5
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
