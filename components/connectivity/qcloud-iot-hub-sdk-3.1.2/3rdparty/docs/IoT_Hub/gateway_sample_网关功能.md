# 网关设备快速入门
本文档将讲述如何在腾讯IoT Hub控制台申请网关设备并绑定子设备, 并结合本 SDK 的**gateway_sample**快速体验网关设备通过 MQTT 协议连接到腾讯云IoT Hub, 代理子设备上下线，发送和接收消息。


## 一. 控制台创建网关设备

#### 1. 创建网关产品和设备
点击页面**创建新产品**按钮, 创建一个产品。

![](https://main.qcloudimg.com/raw/a0da21dc6ac9a9e1dede0077d40cfb22.png)

在弹出的产品窗口中，选择节点类型和产品类型，输入产品名称，选择认证方式和数据格式，输入产品描述，然后点击确定创建。普通网关设备，可按下图选择。

![](https://main.qcloudimg.com/raw/d3f5de3bd07a779f9b1306085fa4d1f1.png)

在生成的产品页面下，点击**设备列表**页面添加新设备。

![](https://main.qcloudimg.com/raw/0530e0da724cd36baefc7011ebce4775.png)

如果产品认证方式为证书认证，输入设备名称成功后，切记点击弹窗中的**下载**按钮，下载所得包中的设备密钥文件和设备证书用于设备连接物联网通信的鉴权。

![](https://main.qcloudimg.com/raw/6592056f1b55fa9262e4b2ab31d0b218.png)

如果产品认证方式为密钥认证，输入设备名称成功后，会在弹窗中显示新添加设备的密钥

![](https://main.qcloudimg.com/raw/fe7a013b1d8c29c477d0ed6d00643751.png)

网关产品，除了自身创建产品和设备，当需要代理子设备进行通讯时，还需要按照普通产品的方式先创建子产品和子设备，并在网关产品页面添加子产品和子设备。需要注意的是子设备是无法直连物联网平台的产品，由网关设备代理连接，所以子设备的认证方式不影响连接，由网关设备来负责认证接入。
创建好子产品之后，先到网关产品页面的子产品栏目添加子产品

![](https://main.qcloudimg.com/raw/00da59942515b1d772323c7087f627e3.png)

再到网关设备页面的子设备栏目添加子设备

![](https://main.qcloudimg.com/raw/c24938ac8ed3aa3e0834cb40598740ca.png)

#### 2. 创建可订阅可发布的Topic

进入产品设置页面的后, 点击权限列表，再点击**添加Topic权限**。

![](https://main.qcloudimg.com/raw/65a2d1b7251de37ce1ca2ba334733c57.png)

在弹窗中输入 data, 并设置操作权限为**发布和订阅**，点击创建。

![](https://main.qcloudimg.com/raw/f429b32b12e3cb0cf319b1efe11ccceb.png)

随后将会创建出 productID/\${deviceName}/data 的 Topic，在产品页面的权限列表中可以查看该产品的所有权限。

对于网关产品，除了添加本产品的Topic权限，还需要为子产品添加Topic权限

![](https://main.qcloudimg.com/raw/3de74cfd5b235fe942fe18c359ad08af.png)

## 二. 编译运行示例程序

#### 1. 编译 SDK
修改CMakeLists.txt确保以下选项存在(以**密钥认证设备**为例)
```
set(BUILD_TYPE                   "release")
set(COMPILE_TOOLS                "gcc") 
set(PLATFORM 	                "linux")
set(FEATURE_MQTT_COMM_ENABLED ON)
set(FEATURE_GATEWAY_ENABLED ON)
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
将上面在腾讯云物联网IoT Hub创建的网关设备和子设备的设备信息(以**密钥认证设备**为例)填写到device_info.json中
```
{
    "auth_mode":"KEY",	
	"productId":"NINEPLMEB6",
  	"deviceName":"Gateway-demo",	
    "key_deviceinfo":{    
        "deviceSecret":"vX23qEazsGsMyf5SMfs6OA6y"
    }
	
	"subDev":{
        "sub_productId":"S3EUVBRJLB",
        "sub_devName":"test_device"
    }
}
```

#### 3. 执行网关示例程序
如下日志信息显示示例程序通过MQTT网关代理子设备上下线状态变化，发布和订阅消息成功。
```
./gateway_sample
INF|2019-09-16 14:35:34|device.c|iot_device_info_set(67): SDK_Ver: 3.1.0, Product_ID: NINEPLMEB6, Device_Name: Gateway-demo
DBG|2019-09-16 14:35:34|HAL_TLS_mbedtls.c|HAL_TLS_Connect(204): Setting up the SSL/TLS structure...
DBG|2019-09-16 14:35:34|HAL_TLS_mbedtls.c|HAL_TLS_Connect(246): Performing the SSL/TLS handshake...
DBG|2019-09-16 14:35:34|HAL_TLS_mbedtls.c|HAL_TLS_Connect(247): Connecting to /NINEPLMEB6.iotcloud.tencentdevices.com/8883...
INF|2019-09-16 14:35:34|HAL_TLS_mbedtls.c|HAL_TLS_Connect(269): connected with /NINEPLMEB6.iotcloud.tencentdevices.com/8883...
INF|2019-09-16 14:35:34|mqtt_client.c|IOT_MQTT_Construct(125): mqtt connect with id: Senw3 success
DBG|2019-09-16 14:35:34|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=$gateway/operation/result/NINEPLMEB6/Gateway-demo|packet_id=27802
DBG|2019-09-16 14:35:34|gateway_api.c|_gateway_event_handler(23): gateway sub|unsub(3) success, packet-id=27802
DBG|2019-09-16 14:35:34|gateway_api.c|IOT_Gateway_Subdev_Online(125): there is no session, create a new session
DBG|2019-09-16 14:35:34|mqtt_client_publish.c|qcloud_iot_mqtt_publish(337): publish packetID=0|topicName=$gateway/operation/NINEPLMEB6/Gateway-demo|payload={"type":"online","payload":{"devices":[{"product_id":"S3EUVBRJLB","device_name":"test_device"}]}}
INF|2019-09-16 14:35:35|gateway_common.c|_gateway_message_handler(134): client_id(S3EUVBRJLB/test_device), online success. result 0
DBG|2019-09-16 14:35:35|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=S3EUVBRJLB/test_device/data|packet_id=27803
DBG|2019-09-16 14:35:35|gateway_api.c|_gateway_event_handler(23): gateway sub|unsub(3) success, packet-id=27803
INF|2019-09-16 14:35:35|gateway_sample.c|_event_handler(61): subscribe success, packet-id=27803
DBG|2019-09-16 14:35:35|mqtt_client_publish.c|qcloud_iot_mqtt_publish(329): publish topic seq=27804|topicName=S3EUVBRJLB/test_device/data|payload={"data":"test gateway"}
INF|2019-09-16 14:35:35|gateway_sample.c|_event_handler(88): publish success, packet-id=27804
INF|2019-09-16 14:35:35|gateway_sample.c|_message_handler(111): Receive Message With topicName:S3EUVBRJLB/test_device/data, payload:{"data":"test gateway"}
DBG|2019-09-16 14:35:36|mqtt_client_publish.c|qcloud_iot_mqtt_publish(337): publish packetID=0|topicName=$gateway/operation/NINEPLMEB6/Gateway-demo|payload={"type":"offline","payload":{"devices":[{"product_id":"S3EUVBRJLB","device_name":"test_device"}]}}
INF|2019-09-16 14:35:36|gateway_common.c|_gateway_message_handler(139): client_id(S3EUVBRJLB/test_device), offline success. result 0
INF|2019-09-16 14:35:37|mqtt_client_connect.c|qcloud_iot_mqtt_disconnect(437): mqtt disconnect!
INF|2019-09-16 14:35:37|mqtt_client.c|IOT_MQTT_Destroy(186): mqtt release!
```

#### 4. 观察子设备上线
如下日志信息显示示例程序的网关设备Gateway-demo通过MQTT代理子设备test_device的上线成功：
```
DBG|2019-09-16 14:35:34|mqtt_client_publish.c|qcloud_iot_mqtt_publish(337): publish packetID=0|topicName=$gateway/operation/NINEPLMEB6/Gateway-demo|payload={"type":"online","payload":{"devices":[{"product_id":"S3EUVBRJLB","device_name":"test_device"}]}}
INF|2019-09-16 14:35:35|gateway_common.c|_gateway_message_handler(134): client_id(S3EUVBRJLB/test_device), online success. result 0
```

#### 5. 观察网关发送接收子设备的消息
如下日志信息显示示例程序的网关设备Gateway-demo通过MQTT代理子设备test_device的消息发送接收
```
DBG|2019-09-16 14:35:35|mqtt_client_publish.c|qcloud_iot_mqtt_publish(329): publish topic seq=27804|topicName=S3EUVBRJLB/test_device/data|payload={"data":"test gateway"}
INF|2019-09-16 14:35:35|gateway_sample.c|_event_handler(88): publish success, packet-id=27804
INF|2019-09-16 14:35:35|gateway_sample.c|_message_handler(111): Receive Message With topicName:S3EUVBRJLB/test_device/data, payload:{"data":"test gateway"}
```