# C-SDK API函数及可变参数说明

设备端 C-SDK 供用户调用的API函数声明，常量以及可变参数定义等头文件位于include目录下面，本文档主要对该目录下面的可变参数以及API函数进行说明。

## 可变参数配置

C-SDK 的使用可以根据具体场景需求，配置相应的参数，满足实际业务的运行。可变接入参数包括：
1. MQTT 阻塞调用(包括连接, 订阅, 发布等)的超时时间, 单位: 毫秒。 建议 5000 毫秒
2. MQTT 协议发送消息和接受消息的 buffer 大小默认是 2048 字节，目前云端一条MQTT消息最大长度为 16 KB
3. COAP 协议发送消息和接受消息的 buffer 大小默认是 512 字节，目前云端一条COAP消息最大长度为 1 KB
4. MQTT 心跳消息发送周期, 最大值为690秒，单位: 毫秒
5. 重连最大等待时间，单位：毫秒。设备断线重连时，若失败则等待时间会翻倍，当超过该最大等待时间则退出重连

修改 include/qcloud_iot_export_variables.h 文件如下宏定义可以改变对应接入参数的配置。
修改完需要重新编译 SDK 
```
/* default MQTT/CoAP timeout value when connect/pub/sub (unit: ms) */
#define QCLOUD_IOT_MQTT_COMMAND_TIMEOUT                             (5 * 1000)

/* default MQTT keep alive interval (unit: ms) */
#define QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL                         (240 * 1000)

/* default MQTT Tx buffer size, MAX: 16*1024 */
#define QCLOUD_IOT_MQTT_TX_BUF_LEN                                  (2048)

/* default MQTT Rx buffer size, MAX: 16*1024 */
#define QCLOUD_IOT_MQTT_RX_BUF_LEN                                  (2048)

/* default COAP Tx buffer size, MAX: 1*1024 */
#define COAP_SENDMSG_MAX_BUFLEN                                     (512)

/* default COAP Rx buffer size, MAX: 1*1024 */
#define COAP_RECVMSG_MAX_BUFLEN                                     (512)

/* MAX MQTT reconnect interval (unit: ms) */
#define MAX_RECONNECT_WAIT_INTERVAL                                 (60 * 1000)
```

## API 函数说明
以下是 C-SDK v3.1.0版本提供的主要功能和对应 API 接口说明，用于客户编写业务逻辑，更加详细的说明如接口参数及返回值可查看SDK代码 include/exports/qcloud_iot_export_*.h 等头文件中的注释

### 1. MQTT 接口

| 序号 | 函数名               | 说明                                            |
| ---- | -------------------- | ----------------------------------------------- |
| 1    | IOT_MQTT_Construct   | 构造 MQTTClient 并连接 MQTT 云端服务                |
| 2    | IOT_MQTT_Destroy     | 关闭 MQTT 连接并销毁 MQTTClient                 |
| 3    | IOT_MQTT_Yield       | 在当前线程上下文中，进行 MQTT 报文读取，消息处理，超时请求，心跳包及重连状态管理等任务 |
| 4    | IOT_MQTT_Publish     | 发布 MQTT 消息                                  |
| 5    | IOT_MQTT_Subscribe   | 订阅 MQTT 主题                                  |
| 6    | IOT_MQTT_Unsubscribe | 取消订阅已订阅的 MQTT 主题                      |
| 7    | IOT_MQTT_IsConnected | 查看当前 MQTT 是否已连接                        |
| 8    | IOT_MQTT_GetErrCode  | 获取IOT_MQTT_Construct失败的错误码              |

- 接口使用说明
```
MQTT构造时候除了提供设备信息，还需要提供一个回调函数，用于接收消息包括连接状态通知，订阅主题是否成功，QoS1消息是否发布成功等等事件通知。订阅主题时则需提供另一个回调函数，用于接收该主题的消息下发。具体接口使用方式可以参考docs/IoT_Hub目录的mqtt_sample_快速入门文档。
```

- 多线程环境使用说明
SDK对于MQTT接口在多线程环境下的使用有如下注意事项：
```
1. 不允许多线程调用IOT_MQTT_Yield，IOT_MQTT_Construct以及IOT_MQTT_Destroy
2. 可以多线程调用IOT_MQTT_Publish，IOT_MQTT_Subscribe及IOT_MQTT_Unsubscribe
3. IOT_MQTT_Yield 作为从socket读取并处理MQTT报文的函数，应保证一定的执行时间，避免被长时间挂起或抢占
```

### 2. 设备影子接口
关于设备影子功能介绍，可以参考[设备影子详情](https://cloud.tencent.com/document/product/634/11918)

| 序号  | 函数名                                             | 说明                                            |
| ---- | -------------------------------------------------- | ---------------------------------------------- |
| 1    | IOT_Shadow_Construct                               | 构造设备影子客户端 ShadowClient，并连接 MQTT 云端服务         |
| 2    | IOT_Shadow_Publish                                 | 影子客户端发布 MQTT 消息                                  |
| 3    | IOT_Shadow_Subscribe                               | 影子客户端订阅 MQTT 主题                                  |
| 4    | IOT_Shadow_Unsubscribe                             | 影子客户端取消订阅已订阅的 MQTT 主题                         |
| 5    | IOT_Shadow_IsConnected                             | 查看当前影子客户端的 MQTT 是否已连接                       |
| 6    | IOT_Shadow_Destroy                                 | 关闭 Shadow MQTT 连接并销毁 ShadowClient              |
| 7    | IOT_Shadow_Yield                                   | 在当前线程上下文中，进行 MQTT 报文读取，消息处理，超时请求，心跳包及重连状态管理等任务  |
| 8    | IOT_Shadow_Update                                  | 异步更新设备影子文档                              |
| 9    | IOT_Shadow_Update_Sync                             | 同步方式更新设备影子文档                           |
| 10   | IOT_Shadow_Get                                     | 异步方式获取设备影子文档                           |
| 11   | IOT_Shadow_Get_Sync                                | 同步方式获取设备影子文档                           |
| 12   | IOT_Shadow_Register_Property                       | 注册当前设备的设备属性                             |
| 13   | IOT_Shadow_UnRegister_Property                     | 删除已经注册过的设备属性                           |
| 14   | IOT_Shadow_JSON_ConstructReport                    | 在JSON文档中添加reported字段，不覆盖更新            |
| 15   | IOT_Shadow_JSON_Construct_OverwriteReport          | 在JSON文档中添加reported字段，覆盖更新              |
| 16   | IOT_Shadow_JSON_ConstructReportAndDesireAllNull    | 在JSON文档中添加reported字段，同时清空desired字段   |
| 17   | IOT_Shadow_JSON_ConstructDesireAllNull             | 在JSON文档中添加 "desired": null 字段             |

### 3. CoAP 接口

| 序号 | 函数名                       | 说明                                                |
| ---- | ---------------------------- | ------------------------------------------------- |
| 1    | IOT_COAP_Construct           | 构造 CoAPClient 并完成 CoAP 连接                   |
| 2    | IOT_COAP_Destroy             | 关闭 CoAP 连接并销毁 CoAPClient                    |
| 3    | IOT_COAP_Yield               | 在当前线程上下文中，进行 CoAP 报文读取和消息处理等任务      |
| 4    | IOT_COAP_SendMessage         | 发布 CoAP 消息                                    |
| 5    | IOT_COAP_GetMessageId        | 获取 COAP Response 消息 msgId                     |
| 6    | IOT_COAP_GetMessagePayload   | 获取 COAP Response 消息内容                        |
| 7    | IOT_COAP_GetMessageCode      | 获取 COAP Response 消息错误码                      |

### 4. OTA 接口
关于 OTA 固件下载功能介绍，可以参考[设备固件升级](https://cloud.tencent.com/document/product/634/14674)

| 序号  | 函数名                        | 说明                                              |
| ---- | ---------------------------- | ------------------------------------------------- |
| 1    | IOT_OTA_Init                 | 初始化 OTA 模块，客户端在调用此接口之前需要先进行 MQTT/COAP 的初始化     |
| 2    | IOT_OTA_Destroy              | 释放 OTA 模块相关的资源                              |
| 3    | IOT_OTA_ReportVersion        | 向 OTA 服务器报告本地固件版本信息                         |
| 4    | IOT_OTA_IsFetching           | 检查是否处于下载固件的状态                            |
| 5    | IOT_OTA_IsFetchFinish        | 检查固件是否已经下载完成                              |
| 6    | IOT_OTA_FetchYield           | 从具有特定超时值的远程服务器获取固件                    |
| 7    | IOT_OTA_Ioctl                | 获取指定的 OTA 信息                                 |
| 8    | IOT_OTA_GetLastError         | 获取最后一个错误代码                                 |
| 9    | IOT_OTA_StartDownload        | 根据获取到的固件更新地址以及本地固件信息偏移（是否断点续传），与固件服务器建立http连接     |
| 10   | IOT_OTA_UpdateClientMd5      | 断点续传前，计算本地固件的MD5                              |
| 11   | IOT_OTA_ReportUpgradeBegin   | 当进行固件升级前，向服务器上报即将升级的状态   |
| 12   | IOT_OTA_ReportUpgradeSuccess | 当固件升级成功之后，向服务器上报升级成功的状态                         |
| 13   | IOT_OTA_ReportUpgradeFail    | 当固件升级失败之后，向服务器上报升级失败的状态        |

### 5. 日志接口
设备日志上报云端功能的详细说明可以参考SDK docs目录下物联网通信平台文档设备日志上报功能部分

| 序号 | 函数名                     | 说明                                          |
| ---- | -------------------------- | --------------------------------------------- |
| 1    | IOT_Log_Set_Level          | 设置 SDK 日志的打印等级                            |
| 2    | IOT_Log_Get_Level          | 返回 SDK 日志打印的等级                            |
| 3    | IOT_Log_Set_MessageHandler | 设置日志回调函数，重定向 SDK 日志于其它输出方式  |
| 4    | IOT_Log_Init_Uploader      | 开启 SDK 日志上报云端的功能并初始化资源                |
| 5    | IOT_Log_Fini_Uploader      | 停止 SDK 日志上报云端功能并释放资源                      |
| 6    | IOT_Log_Upload             | 将 SDK 运行日志上报到云端                         |
| 7    | IOT_Log_Set_Upload_Level   | 设置 SDK 日志的上报等级                            |
| 8    | IOT_Log_Get_Upload_Level   | 返回 SDK 日志上报的等级                            |
| 9    | Log_d/i/w/e                | 按级别打印添加 SDK 日志的接口                         |

### 6. 系统时间接口

| 序号  | 函数名                      | 说明                                              |
| ---- | ---------------------------- | ------------------------------------------------- |
| 1    | IOT_Get_SysTime              | 获取 IoT hub 后台系统时间，目前仅支持 MQTT 通道对时功能 |

### 7. 网关功能接口
关于网关功能介绍，可以参考SDK docs目录下物联网通信平台文档网关产品部分

| 序号 | 函数名                       | 说明                                                |
| ---- | ---------------------------- | ------------------------------------------------- |
| 1    | IOT_Gateway_Construct        | 构造 Gateway client 并完成 MQTT 连接                   |
| 2    | IOT_Gateway_Destroy          | 关闭 MQTT 连接并销毁 Gateway client                    |
| 3    | IOT_Gateway_Subdev_Online    | 子设备上线              |
| 4    | IOT_Gateway_Subdev_Offline   | 子设备下线                                    |
| 3    | IOT_Gateway_Yield            | 在当前线程上下文中，进行 MQTT 报文读取，消息处理，超时请求，心跳包及重连状态管理等任务 |
| 4    | IOT_Gateway_Publish          | 发布 MQTT 消息                                  |
| 5    | IOT_Gateway_Subscribe        | 订阅 MQTT 主题                                  |
| 6    | IOT_Gateway_Unsubscribe      | 取消订阅已订阅的 MQTT 主题                      |

