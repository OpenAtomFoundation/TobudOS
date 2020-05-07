# 设备影子
本文档介绍设备影子功能，并结合 SDK 的**shadow_sample**展示影子的数据流和功能。

## 设备影子介绍

设备影子本质上是一份在服务器端缓存的设备数据（JSON 形式），主要用于保存：

- 设备的当前配置
- 设备的当前状态

作为中介，设备影子可以有效实现设备和用户应用之间的数据双向同步：

- 对于设备配置，用户应用不需要直接修改设备，只需要修改服务器端的设备影子，由设备影子同步到设备。即使当时设备不在线，设备上线后仍能从设备影子同步到最新配置。
- 对于设备状态，设备将状态上报到设备影子，用户应用查询时，只需查询设备影子即可。这样可以有效减少设备和服务器端的网络交互，尤其是低功耗设备。

关于设备影子的详细介绍，请参考[设备影子详情](https://cloud.tencent.com/document/product/634/11918)

## 编译运行示例程序(以**密钥认证设备**为例)

#### 1. 编译 SDK 及示例
修改CMakeLists.txt确保以下选项存在
```
set(BUILD_TYPE                   "release")
set(COMPILE_TOOLS                "gcc") 
set(PLATFORM 	                "linux")
set(FEATURE_MQTT_COMM_ENABLED ON)
set(FEATURE_MQTT_DEVICE_SHADOW ON)
set(FEATURE_AUTH_MODE "KEY")
set(FEATURE_AUTH_WITH_NOTLS OFF)
set(FEATURE_DEBUG_DEV_INFO_USED  OFF)
```
执行脚本编译
```
./cmake_build.sh 
```
示例输出shadow_sample位于`output/release/bin`文件夹中

#### 2. 填写设备信息
将设备信息填写到配置文件device_info.json中
```
{
  "auth_mode":"KEY",	
  "productId":"S3EUVBRJLB",
  "deviceName":"test_device",	
  "key_deviceinfo":{    
      "deviceSecret":"vX6PQqazsGsMyf5SMfs6OA6y"
  }
}
```
#### 3. 第一次执行shadow_sample示例程序
示例首先订阅了$shadow/operation/result/{productID}/{deviceName}主题，然后通过向$shadow/operation/{productID}/{deviceName}主题发送shadow GET命令来获取云端缓存的设备状态，然后就循环更新updateCount变量并执行shadow UPDATE。
假设在updateCount=3的时候停止示例运行。

```
./output/release/bin/shadow_sample 
INF|2019-09-17 12:49:59|device.c|iot_device_info_set(67): SDK_Ver: 3.1.0, Product_ID: S3EUVBRJLB, Device_Name: test_device
DBG|2019-09-17 12:49:59|HAL_TLS_mbedtls.c|HAL_TLS_Connect(204): Setting up the SSL/TLS structure...
DBG|2019-09-17 12:49:59|HAL_TLS_mbedtls.c|HAL_TLS_Connect(246): Performing the SSL/TLS handshake...
DBG|2019-09-17 12:49:59|HAL_TLS_mbedtls.c|HAL_TLS_Connect(247): Connecting to /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2019-09-17 12:49:59|HAL_TLS_mbedtls.c|HAL_TLS_Connect(269): connected with /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2019-09-17 12:50:00|mqtt_client.c|IOT_MQTT_Construct(125): mqtt connect with id: bph30 success
DBG|2019-09-17 12:50:00|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=$shadow/operation/result/S3EUVBRJLB/test_device|packet_id=46510
DBG|2019-09-17 12:50:00|shadow_client.c|_shadow_event_handler(63): shadow subscribe success, packet-id=46510
INF|2019-09-17 12:50:00|shadow_client.c|IOT_Shadow_Construct(172): Sync device data successfully
DBG|2019-09-17 12:50:00|shadow_client.c|IOT_Shadow_Get(383): GET Request Document: {"clientToken":"S3EUVBRJLB-0"}
DBG|2019-09-17 12:50:00|mqtt_client_publish.c|qcloud_iot_mqtt_publish(337): publish packetID=0|topicName=$shadow/operation/S3EUVBRJLB/test_device|payload={"type":"get", "clientToken":"S3EUVBRJLB-0"}
DBG|2019-09-17 12:50:00|shadow_client_manager.c|_on_operation_result_handler(278): type:get
DBG|2019-09-17 12:50:00|shadow_client.c|_update_ack_cb(114): requestAck=0
DBG|2019-09-17 12:50:00|shadow_client.c|_update_ack_cb(117): Received Json Document={"clientToken":"S3EUVBRJLB-0","payload":{"state":{"reported":{}},"timestamp":1568695800,"version":0},"result":0,"timestamp":1568695800,"type":"get"}
DBG|2019-09-17 12:50:00|shadow_client.c|IOT_Shadow_Update(317): UPDATE Request Document: {"state":{"reported":{"updateCount":0}}, "clientToken":"S3EUVBRJLB-1"}
DBG|2019-09-17 12:50:00|mqtt_client_publish.c|qcloud_iot_mqtt_publish(337): publish packetID=0|topicName=$shadow/operation/S3EUVBRJLB/test_device|payload={"type":"update", "state":{"reported":{"updateCount":0}}, "clientToken":"S3EUVBRJLB-1"}
DBG|2019-09-17 12:50:01|shadow_client_manager.c|_on_operation_result_handler(278): type:update
INF|2019-09-17 12:50:01|shadow_sample.c|OnShadowUpdateCallback(49): recv shadow update response, response ack: 0
DBG|2019-09-17 12:50:01|shadow_client.c|IOT_Shadow_Update(317): UPDATE Request Document: {"state":{"reported":{"updateCount":1}}, "clientToken":"S3EUVBRJLB-2"}
DBG|2019-09-17 12:50:01|mqtt_client_publish.c|qcloud_iot_mqtt_publish(337): publish packetID=0|topicName=$shadow/operation/S3EUVBRJLB/test_device|payload={"type":"update", "state":{"reported":{"updateCount":1}}, "clientToken":"S3EUVBRJLB-2"}
DBG|2019-09-17 12:50:02|shadow_client_manager.c|_on_operation_result_handler(278): type:update
INF|2019-09-17 12:50:02|shadow_sample.c|OnShadowUpdateCallback(49): recv shadow update response, response ack: 0
DBG|2019-09-17 12:50:02|shadow_client.c|IOT_Shadow_Update(317): UPDATE Request Document: {"state":{"reported":{"updateCount":2}}, "clientToken":"S3EUVBRJLB-3"}
DBG|2019-09-17 12:50:02|mqtt_client_publish.c|qcloud_iot_mqtt_publish(337): publish packetID=0|topicName=$shadow/operation/S3EUVBRJLB/test_device|payload={"type":"update", "state":{"reported":{"updateCount":2}}, "clientToken":"S3EUVBRJLB-3"}
DBG|2019-09-17 12:50:03|shadow_client_manager.c|_on_operation_result_handler(278): type:update
INF|2019-09-17 12:50:03|shadow_sample.c|OnShadowUpdateCallback(49): recv shadow update response, response ack: 0
DBG|2019-09-17 12:50:04|shadow_client.c|IOT_Shadow_Update(317): UPDATE Request Document: {"state":{"reported":{"updateCount":3}}, "clientToken":"S3EUVBRJLB-4"}
DBG|2019-09-17 12:50:04|mqtt_client_publish.c|qcloud_iot_mqtt_publish(337): publish packetID=0|topicName=$shadow/operation/S3EUVBRJLB/test_device|payload={"type":"update", "state":{"reported":{"updateCount":3}}, "clientToken":"S3EUVBRJLB-4"}
```
#### 4. 查看云端设备影子状态变化并修改状态
从控制台的虚拟设备，可以看到该设备的updateCount已经更新为3，即便设备已经离线，仍然可以通过这份缓存来获取设备离线之前的最新状态。
![](https://main.qcloudimg.com/raw/ec18636eb38ee25ae21389a0004cd094.png)

这个时候如果点击修改，则可以改变影子状态，这个操作相当于模拟设备离线期间，通过云API修改设备的目标状态，比如将updateCount更改为5

![](https://main.qcloudimg.com/raw/9719fa972cbf158165ae208113e7bc19.png)

确定修改之后，再次运行shadow_sample

#### 5. 第二次执行shadow_sample示例程序
这时候可以看到当执行shadow GET命令来获取云端缓存的设备状态，updateCount已经变为云端最新的状态。
``` 
./output/release/bin/shadow_sample 
INF|2019-09-17 12:59:01|device.c|iot_device_info_set(67): SDK_Ver: 3.1.0, Product_ID: S3EUVBRJLB, Device_Name: test_device
DBG|2019-09-17 12:59:01|HAL_TLS_mbedtls.c|HAL_TLS_Connect(204): Setting up the SSL/TLS structure...
DBG|2019-09-17 12:59:01|HAL_TLS_mbedtls.c|HAL_TLS_Connect(246): Performing the SSL/TLS handshake...
DBG|2019-09-17 12:59:01|HAL_TLS_mbedtls.c|HAL_TLS_Connect(247): Connecting to /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2019-09-17 12:59:01|HAL_TLS_mbedtls.c|HAL_TLS_Connect(269): connected with /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2019-09-17 12:59:01|mqtt_client.c|IOT_MQTT_Construct(125): mqtt connect with id: hY8DA success
DBG|2019-09-17 12:59:01|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=$shadow/operation/result/S3EUVBRJLB/test_device|packet_id=63420
DBG|2019-09-17 12:59:01|shadow_client.c|_shadow_event_handler(63): shadow subscribe success, packet-id=63420
INF|2019-09-17 12:59:01|shadow_client.c|IOT_Shadow_Construct(172): Sync device data successfully
DBG|2019-09-17 12:59:01|shadow_client.c|IOT_Shadow_Get(383): GET Request Document: {"clientToken":"S3EUVBRJLB-0"}
DBG|2019-09-17 12:59:01|mqtt_client_publish.c|qcloud_iot_mqtt_publish(337): publish packetID=0|topicName=$shadow/operation/S3EUVBRJLB/test_device|payload={"type":"get", "clientToken":"S3EUVBRJLB-0"}
DBG|2019-09-17 12:59:01|shadow_client_manager.c|_on_operation_result_handler(278): type:get
DBG|2019-09-17 12:59:01|shadow_client.c|_update_ack_cb(114): requestAck=0
DBG|2019-09-17 12:59:01|shadow_client.c|_update_ack_cb(117): Received Json Document={"clientToken":"S3EUVBRJLB-0","payload":{"state":{"reported":{"updateCount":5}},"timestamp":1568696289949,"version":5},"result":0,"timestamp":1568696341,"type":"get"}
```
