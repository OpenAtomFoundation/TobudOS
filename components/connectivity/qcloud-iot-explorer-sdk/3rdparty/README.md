#  腾讯云物联网开发平台设备端 C-SDK 
腾讯云物联网开发平台（IoT Explorer）设备端 C-SDK ，配合平台对设备数据模板化的定义，实现和云端基于数据模板协议的数据交互框架，开发者基于IoT_Explorer C-SDK数据模板框架，通过脚本自动生成模板代码，快速实现设备和平台、设备和应用之间的数据交互。

## IoT_Explorer C-SDK 架构

#### 架构图

![framework](https://main.qcloudimg.com/raw/76fc3f15c4c91ea6cf7e496f25d5d572.jpg)

#### 架构说明

SDK分四层设计，从上至下分别为平台服务层、核心协议层、网络层、硬件抽象层。

- 服务层
在网络协议层之上，实现了包括设备接入鉴权，设备影子，数据模板，网关，动态注册，日志上报和OTA等功能，并提供相关API接口给用户使用，关于服务层API说明，请参考doc目录文档[C-SDK_API及可变参数说明](https://github.com/tencentyun/qcloud-iot-explorer-sdk-embedded-c/blob/master/docs/C-SDK_API%E5%8F%8A%E5%8F%AF%E5%8F%98%E5%8F%82%E6%95%B0%E8%AF%B4%E6%98%8E.md)

- 协议层
设备端和IoT平台交互的网络协议包括MQTT/COAP/HTTP

- 网络层
实现基于TLS/SSL(TLS/DTLS)方式，Bsd_socket(TCP/UDP)方式和AT_socket方式的网络协议栈，不同服务可根据需要使用不同的协议栈接口函数。

- 硬件抽象层
实现对不同硬件平台的底层操作的抽象封装，需要针对具体的软硬件平台开展移植，分为必须实现和可选实现两部分HAL层接口。

## SDK 目录结构简介

#### 目录结构及顶层文件简介

| 名称               | 说明                                                         |
| ------------------ | ------------------------------------------------------------ |
| CMakeLists.txt     | cmake编译描述文件                                            |
| CMakeSettings.json | visual studio下的cmake配置文件                               |
| cmake_build.sh     | Linux下使用cmake的编译脚本                                   |
| make.settings      | Linux下使用Makefile直接编译的配置文件                        |
| Makefile           | Linux下使用Makefile直接编译                                  |
| device_info.json   | 设备信息文件，当DEBUG_DEV_INFO_USED=OFF时，将从该文件解析出设备信息 |
| docs               | 文档目录，SDK使用说明文档。                      |
| external_libs      | 第三方软件包组件，如mbedtls                                  |
| samples            | 应用示例                                                     |
| include            | 提供给用户使用的外部头文件                                   |
| platform           | 平台相关的源码文件，目前提供了针对不同OS(Linux/Windows/FreeRTOS/nonOS)，TLS(mbedtls)以及AT模组下的实现。 |
| sdk_src            | SDK核心通信协议及服务代码                                    |
| tools              | SDK配套的编译及代码生成脚本工具                              |

## SDK 编译方式说明

C-SDK 支持三种编译方式：

- cmake 方式
- Makefile 方式
- 代码抽取方式

编译方式以及编译配置选项的详细说明请参考docs目录文档[C-SDK_Build编译环境及配置选项说明](https://github.com/tencentyun/qcloud-iot-explorer-sdk-embedded-c/blob/master/docs/C-SDK_Build%E7%BC%96%E8%AF%91%E7%8E%AF%E5%A2%83%E5%8F%8A%E9%85%8D%E7%BD%AE%E9%80%89%E9%A1%B9%E8%AF%B4%E6%98%8E.md) 

## SDK 快速体验

请参考docs目录文档 [IoT_Explorer开发平台快速入门](https://github.com/tencentyun/qcloud-iot-explorer-sdk-embedded-c/blob/master/docs/IoT_Explorer%E5%BC%80%E5%8F%91%E5%B9%B3%E5%8F%B0%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8.md)

