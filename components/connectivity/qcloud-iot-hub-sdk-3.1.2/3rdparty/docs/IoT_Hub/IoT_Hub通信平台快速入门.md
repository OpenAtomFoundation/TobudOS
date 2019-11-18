# 腾讯云物联网通信 C-SDK
腾讯云物联网通信IoT Hub C-SDK 依靠安全且性能强大的数据通道，为物联网领域开发人员提供设备终端和云端的双向通信能力。
本文档介绍SDK的各项功能。

## 快速入门
请参考**mqtt_sample_快速入门.md**文档，介绍如何在腾讯云物联网通信IoT Hub控制台申请设备, 并结合本 SDK 的**mqtt_sample**快速体验设备端通过 MQTT 协议连接到腾讯云IoT Hub, 发送和接收消息。

## 设备影子
请参考**shadow_sample_设备影子.md**文档，介绍设备影子功能，并结合 SDK 的**shadow_sample**展示影子的数据流和功能

## 网关功能 
请参考**gateway_sample_网关功能.md**文档，介绍如何在腾讯IoT Hub控制台申请网关设备并绑定子设备, 并结合本 SDK 的**gateway_sample**快速体验网关设备通过 MQTT 协议连接到腾讯云IoT Hub, 代理子设备上下线，发送和接收消息。

## 设备互通
请参考**door_aircond_sample_设备互通.md**文档，介绍一个智能家居设备互通的场景, 结合本 SDK 的**aircond_shadow_sample**和**door_mqtt_sample**快速体验基于IoT Hub的消息转发和规则引擎实现设备之间的联动。

## 动态注册
请参考**dynreg_dev_sample_动态注册.md**文档，介绍腾讯云物联网的设备接入认证以及动态注册功能。

## 设备日志上报功能
设备端日志上报功能，可将设备端的Log通过HTTP上报到云端，并可在控制台展示，方便用户远程调试、诊断及监控设备运行状况。目前该功能仅支持MQTT模式。
只要将SDK的编译宏FEATURE_LOG_UPLOAD_ENABLED置为y（默认为y），并在控制台设置上报级别，则在代码中调用Log_e/w/i/d接口的日志除了会在终端打印出来，还会上报云端并在控制台展示，如下图。
![](https://main.qcloudimg.com/raw/cae7f9e7cf1e354cfc1e3578eb6746bc.png)

上报级别设置可参见下图，Level级别越大则上报的日志越多，比如Level3(信息)会将ERROR/WARN/INFO级别的日志都上报而DEBUG级别则不上报。控制台默认为关闭状态，则表示设备端仅在MQTT连接失败的时候才会上报ERROR级别日志。
![](https://main.qcloudimg.com/raw/826b648993a267b1cc2f082148d8d073.png)

代码具体用例可以参考mqtt_sample以及qcloud_iot_export_log.h注释说明，用户除了打开编译宏开关，还需要调用IOT_Log_Init_Uploader函数进行初始化。SDK在IOT_MQTT_Yield函数中会定时进行上报，此外，用户可根据自身需要，在程序出错退出的时候调用IOT_Log_Upload(true)强制上报。同时SDK提供在HTTP通讯出错无法上报日志时的缓存和恢复正常后重新上报机制，但需要用户根据设备具体情况提供相关回调函数，如不提供回调或回调函数提供不全则该缓存机制不生效，HTTP通讯失败时日志会被丢掉。

打开日志上报功能，请确保编译配置文件CMakeLists.txt中使能下面选项
```
set(FEATURE_LOG_UPLOAD_ENABLED ON)
```

## CoAP通讯
IoT Hub支持设备端通过CoAP协议接入，请参考[CoAP 协议说明](https://cloud.tencent.com/document/product/634/14063)

编译运行示例程序coap_sample，首先修改CMakeLists.txt确保以下选项存在(以密钥认证设备为例)
```
set(BUILD_TYPE                   "release")
set(COMPILE_TOOLS                "gcc") 
set(PLATFORM 	                "linux")
set(FEATURE_COAP_COMM_ENABLED ON)
set(FEATURE_AUTH_MODE "KEY")
set(FEATURE_AUTH_WITH_NOTLS OFF)
set(FEATURE_DEBUG_DEV_INFO_USED  OFF)
```
编译SDK，将控制台创建的设备信息填入device_info.json中
再执行coap_sample
```
./output/release/bin/coap_sample 
INF|2019-09-16 23:49:36|device.c|iot_device_info_set(67): SDK_Ver: 3.1.0, Product_ID: S3EUVBRJLB, Device_Name: test_device
INF|2019-09-16 23:49:38|coap_client.c|IOT_COAP_Construct(82): coap connect success
INF|2019-09-16 23:49:38|coap_client_message.c|coap_message_send(402): add coap message id: 9734 into wait list ret: 0
DBG|2019-09-16 23:49:38|coap_client_message.c|_coap_message_handle(295): receive coap piggy ACK message, id 9734
INF|2019-09-16 23:49:38|coap_client_auth.c|_coap_client_auth_callback(43): auth token message success, code_class: 2 code_detail: 5
DBG|2019-09-16 23:49:38|coap_client_auth.c|_coap_client_auth_callback(53): auth_token_len = 10, auth_token = YWAIGGUGUC
DBG|2019-09-16 23:49:38|coap_client_message.c|_coap_message_list_proc(146): remove the message id 9734 from list
INF|2019-09-16 23:49:38|coap_client_message.c|_coap_message_list_proc(85): remove node
INF|2019-09-16 23:49:38|coap_client.c|IOT_COAP_Construct(91): device auth successfully, connid: Xy9W9
INF|2019-09-16 23:49:38|coap_sample.c|main(170): topic name is S3EUVBRJLB/test_device/data
INF|2019-09-16 23:49:38|coap_client_message.c|coap_message_send(402): add coap message id: 9735 into wait list ret: 0
DBG|2019-09-16 23:49:38|coap_sample.c|main(177): client topic has been sent, msg_id: 9735
DBG|2019-09-16 23:49:39|coap_client_message.c|_coap_message_handle(295): receive coap piggy ACK message, id 9735
INF|2019-09-16 23:49:39|coap_sample.c|event_handler(78): message received ACK, msgid: 9735
DBG|2019-09-16 23:49:39|coap_client_message.c|_coap_message_list_proc(146): remove the message id 9735 from list
INF|2019-09-16 23:49:39|coap_client_message.c|_coap_message_list_proc(85): remove node
INF|2019-09-16 23:49:39|coap_client.c|IOT_COAP_Destroy(125): coap release!
```

