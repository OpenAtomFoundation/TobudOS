# 文档说明
本文档介绍如何将腾讯云物联 C-SDK 移植到**FreeRTOS+lwIP** 平台。

## FreeRTOS移植简介
FreeRTOS作为一个微内核系统，主要提供任务创建及调度和任务间通信等OS核心机制，在不同设备平台还需要搭配多个软件组件包括C运行库（如newlib或者ARM CMSIS库）和TCP/IP网络协议栈（如lwIP）才能形成完整的嵌入式运行平台。同时各个设备平台的编译开发环境也各不相同，因此在移植C-SDK的时候，需要根据不同设备的具体情况进行适配。
SDK 在**platform/os/freertos**里面提供了一个基于 **FreeRTOS+lwIP+newlib** 的参考实现，该实现已经在乐鑫ESP8266平台上验证测试过。

## 从腾讯云物联 C-SDK 中抽取相关代码

因为基于RTOS系统的平台编译方式各不相同，一般无法直接使用SDK 的cmake或者make编译，因此SDK提供了代码抽取功能，可根据需要将相关代码抽取到一个单独的文件夹，文件夹里面的代码层次目录简洁，方便用户拷贝集成到自己的开发环境。

首先修改 CMakeLists.txt 中配置为freertos平台，并开启代码抽取功能：
```
set(BUILD_TYPE                  "release")
set(PLATFORM 	                "freertos")
set(EXTRACT_SRC ON)
set(FEATURE_AT_TCP_ENABLED OFF)
```
在Linux环境运行以下命令
```
mkdir build
cd build
cmake ..
```
即可在output/qcloud_iot_c_sdk中找到相关代码文件，目录层次如下：
```
 qcloud_iot_c_sdk
 ├── include
 │   ├── config.h
 │   ├── exports
 ├── platform
 └── sdk_src
     └── internal_inc
```
include目录为SDK供用户使用的API及可变参数，其中config.h为根据编译选项生成的编译宏。API具体介绍请参考**C-SDK_API及可变参数说明**。
platform目录为平台相关的代码，可根据设备的具体情况进行修改适配。具体的函数说明请参考文档**C-SDK_Porting跨平台移植概述**
sdk_src为SDK的核心逻辑及协议相关代码，一般不需要修改，其中internal_inc为SDK内部使用的头文件。

用户可将qcloud_iot_c_sdk拷贝到其目标平台的编译开发环境，并根据具体情况修改编译选项。

## 乐鑫ESP8266 RTOS平台移植示例
在Linux开发环境基于乐鑫ESP8266 RTOS平台搭建一个工程示例

### 1. 获取 ESP8266_RTOS_SDK 并创建工程

请参考[ESP8266_RTOS_SDK](https://github.com/espressif/ESP8266_RTOS_SDK) 获取RTOS_SDK 和交叉编译器，并创建一个项目工程

### 2. 拷贝SDK代码

将上面抽取的qcloud_iot_c_sdk目录拷贝到components/qcloud_iot下面

在components/qcloud_iot下面新建一个编译配置文件component.mk，内容如下
```
#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := \
	qcloud_iot_c_sdk/include \
    qcloud_iot_c_sdk/include/exports \
	qcloud_iot_c_sdk/sdk_src/internal_inc

COMPONENT_SRCDIRS := \
	qcloud_iot_c_sdk/sdk_src \
	qcloud_iot_c_sdk/platform
    
```

这样就可以将qcloud_iot_c_sdk作为一个组件进行编译了，之后在用户代码里面就可以调用物联 C-SDK的接口进行连接和收发消息。
