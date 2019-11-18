
# 快速开始
本文档将讲述如何在腾讯云物联网通信IoT Hub控制台申请设备, 并结合本 SDK 的**mqtt_sample**快速体验设备端通过 MQTT 协议连接到腾讯云IoT Hub, 发送和接收消息。

## 一. 控制台创建设备

#### 1. 注册/登录腾讯云账号
访问[腾讯云登录页面](https://cloud.tencent.com/login?s_url=https%3A%2F%2Fcloud.tencent.com%2F), 点击[立即注册](https://cloud.tencent.com/register?s_url=https%3A%2F%2Fcloud.tencent.com%2F), 免费获取腾讯云账号，若您已有账号，可直接登录。

#### 2. 访问物联网通信控制台
登录后点击右上角控制台，进入控制台后，鼠标悬停在云产品上，弹出层叠菜单，点击物联网通信。

![](https://main.qcloudimg.com/raw/f8719b28d557496ce3388e0439ca38b6.png)

或者直接访问[物联网通信控制台](https://console.qcloud.com/iotcloud)

#### 3. 创建产品和设备
点击页面**创建新产品**按钮, 创建一个产品。

![](https://main.qcloudimg.com/raw/a0da21dc6ac9a9e1dede0077d40cfb22.png)

在弹出的产品窗口中，选择节点类型和产品类型，输入产品名称，选择认证方式和数据格式，输入产品描述，然后点击确定创建。对于普通直连设备，可按下图选择。

![](https://main.qcloudimg.com/raw/7ee90122a01f5f277785885669a56aec.png)

在生成的产品页面下，点击**设备列表**页面添加新设备。

![](https://main.qcloudimg.com/raw/0530e0da724cd36baefc7011ebce4775.png)

如果产品认证方式为证书认证，输入设备名称成功后，切记点击弹窗中的**下载**按钮，下载所得包中的设备密钥文件和设备证书用于设备连接物联网通信的鉴权。

![](https://main.qcloudimg.com/raw/6592056f1b55fa9262e4b2ab31d0b218.png)

如果产品认证方式为密钥认证，输入设备名称成功后，会在弹窗中显示新添加设备的密钥

![](https://main.qcloudimg.com/raw/fe7a013b1d8c29c477d0ed6d00643751.png)

#### 4. 创建可订阅可发布的Topic

按照**第三步**中进入产品设置页面的方法进入页面后, 点击权限列表，再点击**添加Topic权限**。

![](https://main.qcloudimg.com/raw/65a2d1b7251de37ce1ca2ba334733c57.png)

在弹窗中输入 data, 并设置操作权限为**发布和订阅**，点击创建。

![](https://main.qcloudimg.com/raw/f429b32b12e3cb0cf319b1efe11ccceb.png)

随后将会创建出 productID/\${deviceName}/data 的 Topic，在产品页面的权限列表中可以查看该产品的所有权限。


## 二. 编译运行示例程序

#### 1. 编译 SDK
修改CMakeLists.txt确保以下选项存在(以密钥认证设备为例)
```
set(BUILD_TYPE                   "release")
set(COMPILE_TOOLS                "gcc") 
set(PLATFORM 	                "linux")
set(FEATURE_MQTT_COMM_ENABLED ON)
set(FEATURE_AUTH_MODE "KEY")
set(FEATURE_AUTH_WITH_NOTLS OFF)
set(FEATURE_DEBUG_DEV_INFO_USED  OFF)
```
执行脚本编译
```
./cmake_build.sh 
```
示例输出位于`output/release/bin`文件夹中

#### 2. 填写设备信息
将上面在腾讯云物联网IoT Hub创建的设备的设备信息(以**密钥认证设备**为例)填写到device_info.json中
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

#### 3. 执行 mqtt_sample 示例程序
```
./output/release/bin/mqtt_sample 
INF|2019-09-12 21:28:20|device.c|iot_device_info_set(67): SDK_Ver: 3.1.0, Product_ID: S3EUVBRJLB, Device_Name: test_device
DBG|2019-09-12 21:28:20|HAL_TLS_mbedtls.c|HAL_TLS_Connect(204): Setting up the SSL/TLS structure...
DBG|2019-09-12 21:28:20|HAL_TLS_mbedtls.c|HAL_TLS_Connect(246): Performing the SSL/TLS handshake...
DBG|2019-09-12 21:28:20|HAL_TLS_mbedtls.c|HAL_TLS_Connect(247): Connecting to /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2019-09-12 21:28:20|HAL_TLS_mbedtls.c|HAL_TLS_Connect(269): connected with /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2019-09-12 21:28:20|mqtt_client.c|IOT_MQTT_Construct(125): mqtt connect with id: p8t0W success
INF|2019-09-12 21:28:20|mqtt_sample.c|main(303): Cloud Device Construct Success
DBG|2019-09-12 21:28:20|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=$sys/operation/result/S3EUVBRJLB/test_device|packet_id=1932
INF|2019-09-12 21:28:20|mqtt_sample.c|_mqtt_event_handler(71): subscribe success, packet-id=1932
DBG|2019-09-12 21:28:20|system_mqtt.c|_system_mqtt_sub_event_handler(80): mqtt sys topic subscribe success
DBG|2019-09-12 21:28:20|mqtt_client_publish.c|qcloud_iot_mqtt_publish(337): publish packetID=0|topicName=$sys/operation/S3EUVBRJLB/test_device|payload={"type": "get", "resource": ["time"]}
DBG|2019-09-12 21:28:20|system_mqtt.c|_system_mqtt_message_callback(63): Recv Msg Topic:$sys/operation/result/S3EUVBRJLB/test_device, payload:{"type":"get","time":1568294900}
INF|2019-09-12 21:28:21|mqtt_sample.c|main(316): system time is 1568294900
DBG|2019-09-12 21:28:21|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=S3EUVBRJLB/test_device/data|packet_id=1933
INF|2019-09-12 21:28:21|mqtt_sample.c|_mqtt_event_handler(71): subscribe success, packet-id=1933
DBG|2019-09-12 21:28:21|mqtt_client_publish.c|qcloud_iot_mqtt_publish(329): publish topic seq=1934|topicName=S3EUVBRJLB/test_device/data|payload={"action": "publish_test", "count": "0"}
INF|2019-09-12 21:28:21|mqtt_sample.c|_mqtt_event_handler(98): publish success, packet-id=1934
INF|2019-09-12 21:28:21|mqtt_sample.c|on_message_callback(195): Receive Message With topicName:S3EUVBRJLB/test_device/data, payload:{"action": "publish_test", "count": "0"}
INF|2019-09-12 21:28:22|mqtt_client_connect.c|qcloud_iot_mqtt_disconnect(437): mqtt disconnect!
INF|2019-09-12 21:28:22|system_mqtt.c|_system_mqtt_sub_event_handler(98): mqtt client has been destroyed
INF|2019-09-12 21:28:22|mqtt_client.c|IOT_MQTT_Destroy(186): mqtt release!
```

#### 4. 观察消息发送
如下日志信息显示示例程序通过 MQTT 的 Publish 类型消息, 上报数据到 /{productID}/{deviceName}/data, 服务器已经收到并成功完成了该消息的处理 
```
INF|2019-09-12 21:28:21|mqtt_sample.c|_mqtt_event_handler(98): publish success, packet-id=1934
```

#### 5. 观察消息接收
如下日志信息显示该消息因为是到达已被订阅的 Topic, 所以又被服务器原样推送到示例程序, 并进入相应的回调函数
```
INF|2019-09-12 21:28:21|mqtt_sample.c|on_message_callback(195): Receive Message With topicName:S3EUVBRJLB/test_device/data, payload:{"action": "publish_test", "count": "0"}
```

#### 6. 观察控制台日志
可以登录物联网通信控制台, 点击左边导航栏中的**云日志**, 查看刚才上报的消息
![](https://main.qcloudimg.com/raw/f589691c7e007b59a5426ede6dc17ddb.png)


