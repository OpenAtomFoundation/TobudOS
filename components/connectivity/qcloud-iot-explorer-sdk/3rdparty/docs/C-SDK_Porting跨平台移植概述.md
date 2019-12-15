## C-SDK跨平台移植

本文档介绍如何将设备端 C-SDK 移植到目标硬件平台。C-SDK 采用模块化设计，分离核心协议服务与硬件抽象层，在进行跨平台移植时，一般只需要对硬件抽象层进行修改适配就可以。

## C-SDK 架构

#### 架构图

![framework](https://main.qcloudimg.com/raw/76fc3f15c4c91ea6cf7e496f25d5d572.jpg)

#### 架构说明

SDK分四层设计，从上至下分别为平台服务层、核心协议层、网络层、硬件抽象层。

- 服务层
在网络协议层之上，实现了包括设备接入鉴权，设备影子，数据模板，网关，动态注册，日志上报和OTA等功能，并提供相关API接口给用户使用，关于服务层API说明，请参考文档[C-SDK_API及可变参数说明]()

- 协议层
设备端和IoT平台交互的网络协议包括MQTT/COAP/HTTP

- 网络层
实现基于TLS/SSL(TLS/DTLS)方式，POSIX_socket(TCP/UDP)方式和AT_socket方式的网络协议栈，不同服务可根据需要使用不同的协议栈接口函数。

- 硬件抽象层
实现对不同硬件平台的底层操作的抽象封装，需要针对具体的软硬件平台开展移植，分为必须实现和可选实现两部分HAL层接口。

## 硬件抽象层移植

HAL层主要有几大块的移植，分别是OS相关的、网络及TLS相关的、时间及打印相关的、设备信息相关的。
SDK在 **platform/os** 目录示例了Linux、Windows、FreeRTOS及nonOS四个场景的硬件抽象层实现，可以参考最相近的目录展开目标平台的移植。

#### OS相关接口

| 序号 | 函数名                 | 说明                                       |
| ---- | ---------------------- | ------------------------------------------ |
| 1    | HAL_Malloc             | 动态申请内存块     |
| 2    | HAL_Free               | 释放内存块                              |
| 3   | HAL_ThreadCreate        | 线程创建                                |
| 4   | HAL_ThreadDestroy        | 线程销毁                               |
| 5   | HAL_MutexCreate        | 创建互斥锁                               |
| 6   | HAL_MutexDestroy       | 销毁互斥锁                               |
| 7   | HAL_MutexLock          | mutex 加锁                               |
| 8   | HAL_MutexUnlock        | mutex 解锁                               |
| 9   | HAL_SemaphoreCreate        | 创建信号量                               |
| 10   | HAL_SemaphoreDestroy        | 销毁信号量                               |
| 11   | HAL_SemaphoreWait        | 等待信号量                               |
| 12   | HAL_SemaphorePost        | 释放信号量                               |
| 13    | HAL_SleepMs            | 休眠                                     |

#### 网络及TLS相关的HAL接口

网络相关接口提供二选一的适配移植。对于具备网络通讯能力并且本身集成TCP/IP网络协议栈的设备，需要实现BSD_socket的网络HAL接口，使用TLS/SSL加密通讯的还需要实现TLS相关的HAL接口。而对于[MCU+通用TCP_AT模组]()的设备，则可以选择SDK提供的AT_Socket框架，并实现相关的AT模组接口。

##### 基于BSD_socket的HAL接口

其中TCP/UDP相关接口基于POSIX socket函数实现。TLS相关接口依赖于 **mbedtls** 库，移植之前必须确保系统上有可用的**mbedtls**库。如果采用其他TLS/SSL库，可参考**platform/tls/mbedtls**相关实现进行移植适配。
UDP/DTLS 相关的函数仅在使能 **COAP** 通讯的时候才需要移植。

| 序号 | 函数名                 | 说明                                       |
| ---- | ---------------------- | ------------------------------------------ |
| 1    | HAL_TCP_Connect               | 建立 TCP 连接                              |
| 2    | HAL_TCP_Disconnect             | 断开 TCP 连接 |
| 3   | HAL_TCP_Write        | TCP 写                                |
| 4   | HAL_TCP_Read        | TCP 读                               |
| 5   | HAL_TLS_Connect        | 建立 TLS 连接              |
| 6   | HAL_TLS_Disconnect     | 断开 TLS 连接                            |
| 7   | HAL_TLS_Write          | TLS 写                   |
| 8   | HAL_TLS_Read           | TLS 读                  |
| 9    | HAL_UDP_Connect               | 建立 TCP 连接                              |
| 10    | HAL_UDP_Disconnect             | 断开 TCP 连接 |
| 11  | HAL_UDP_Write        | UDP 写                                |
| 12   | HAL_UDP_Read        | UPD 读                               |
| 13   | HAL_DTLS_Connect        | 建立 DTLS 连接              |
| 14   | HAL_DTLS_Disconnect     | 断开 DTLS 连接                            |
| 15   | HAL_DTLS_Write          | DTLS 写                   |
| 16   | HAL_DTLS_Read           | DTLS 读                  |

##### 基于AT_socket的HAL接口
通过使能编译宏**AT_TCP_ENABLED**选择AT_socket, 则SDK会调用 network_at_tcp.c 的at_socket接口，at_socket层不需要移植，需要实现AT串口驱动及AT模组驱动，AT模组驱动只需要实现AT框架中at_device的驱动结构体 *at_device_op_t* 的驱动接口即可，可以参照at_device目录下的已支持的模组。AT串口驱动需要实现串口的中断接收，然后在中断服务程序中调用回调函数 *at_client_uart_rx_isr_cb* 即可，可以参考 HAL_AT_UART_freertos.c 实现目标平台的移植。

| 序号 | 函数名                 | 说明                                       |
| ---- | ---------------------- | ------------------------------------------ |
| 1    | HAL_AT_Uart_Init               |初始化 AT串口                           |
| 2    | HAL_AT_Uart_Deinit             | 去初始化 AT串口 |
| 3   | HAL_AT_Uart_Send        | AT串口发送数据                                |
| 4   | HAL_AT_UART_IRQHandler        | AT串口接收中断服务程序                               |


#### 时间及打印相关的HAL接口

| 序号 | 函数名                 | 说明                                       |
| ---- | ---------------------- | ------------------------------------------ |
| 1    | HAL_Printf             | 将格式化的数据写入标准输出流中          |
| 2    | HAL_Snprintf           | 将格式化的数据写入字符串                 |
| 3    | HAL_UptimeMs           | 检索自系统启动以来已运行的毫秒数         |
| 4    | HAL_DelayMs           | 阻塞延时，单位毫秒         |

#### 设备信息相关的HAL接口

接入IoT平台需要在平台创建产品和设备信息，同时需要将产品及设备信息保存在设备侧的非易失存储介质。可以参考 platform/os/linux/HAL_Device_linux.c示例实现。

| 序号 | 函数名                 | 说明                                       |
| ---- | ---------------------- | ------------------------------------------ |
| 1    | HAL_GetDevInfo             | 设备信息读取          |
| 2    | HAL_SetDevInfo           | 设备信息保存                     |

