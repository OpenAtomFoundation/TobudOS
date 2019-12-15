## 简介

对于不具备网络通讯能力的MCU，一般采用MCU+通讯模组的方式，通讯模组（包括WiFi/2G/4G/NB-IoT）一般提供了基于串口的AT指令协议供MCU进行网络通讯。针对这种场景，C-SDK 封装了AT-socket网络层，网络层之上的核心协议和服务层无须移植。本文阐述针对 MCU(无OS)+通用TCP AT模组的目标环境，如何移植C-SDK 并接入腾讯云物联网平台。
相较于有RTOS场景，at_socket网络接收数据的处理会有差异，应用层需要周期性的调用**IOT_MQTT_Yield**来接收服务端下行数据，错过接收窗口则会存在数据丢失的情况，所以在业务逻辑较为复杂的场景建议使用RTOS，通过配置 FEATURE_AT_OS_USED = OFF选择无OS方式。

## 移植步骤

### 1. 下载最新版本设备端 [C-SDK](https://github.com/tencentyun/qcloud-iot-sdk-embedded-c)。

### 2. SDK功能配置及代码抽取
-  无RTOS使用通用TCP模组编译配置选项配置如下:

| 名称                             | 配置        | 说明                                                         |
| :------------------------------- | ------------- | ------------------------------------------------------------ |
| BUILD_TYPE                       | debug/release,根据需要设置 |
| EXTRACT_SRC                      | ON       | 使能代码抽取                                               |
| COMPILE_TOOLS                    | gcc/MSVC      | 根据需要设置，IDE情况不关注            |
| PLATFORM                         | linux/windows | 根据需要设置，IDE情况不关注                |
| FEATURE_OTA_COMM_ENABLED         | ON/OFF       | 根据需要设置                     |
| FEATURE_AUTH_MODE                | KEY      | 资源受限设备认证方式建议选秘钥认证    |
| FEATURE_AUTH_WITH_NOTLS          | ON/OFF        | 根据需要是否使能TLS             |
| FEATURE_AT_TCP_ENABLED           | ON        | 使能at_socket组件                          |
| FEATURE_AT_UART_RECV_IRQ         | ON        | 使能AT串口中断接收                  |
| FEATURE_AT_OS_USED               | OFF        | at_socket组件无RTOS环境使用                         |
| FEATURE_AT_DEBUG                 | OFF      | 默认关闭AT模组调试功能，有调试需要再打开|

- 参考文档[C-SDK_Build编译环境及配置选项说明]()抽取代码

### 3. HAL 层移植

参见[C-SDK_Porting跨平台移植概述]()

对于网络相关的HAL接口，通过上面的编译选项已选择SDK提供的AT_Socket框架，SDK会调用 network_at_tcp.c 的at_socket接口，at_socket层不需要移植，需要实现AT串口驱动及AT模组驱动，AT模组驱动只需要实现AT框架中at_device的驱动结构体 *at_device_op_t* 的驱动接口即可，可以参照at_device目录下的已支持的模组。AT串口驱动需要实现串口的中断接收，然后在中断服务程序中调用回调函数 *at_client_uart_rx_isr_cb* 即可，可以参考 HAL_OS_nonos.c 实现目标平台的移植。

### 4. 业务逻辑开发

可以参考Docs/IoT_Hub目录下的例程。

