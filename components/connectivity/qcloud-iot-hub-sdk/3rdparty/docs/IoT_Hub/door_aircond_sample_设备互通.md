
# 设备互通
本文档根据一个智能家居设备互通的场景, 结合本 SDK 的**aircond_shadow_sample**和**door_mqtt_sample**快速体验基于IoT Hub的消息转发和规则引擎实现设备之间的联动。

## 一. 控制台创建设备

#### 1. 场景说明
本文档的用户场景请参考腾讯云文档中心的[设备互通](https://cloud.tencent.com/document/product/634/11913)
假设一个智能家居的场景，门打开时候触发空调的温度控制，两个设备之间的消息是通过云端规则引擎进行转发的

![](https://mc.qcloudimg.com/static/img/d158634d34fbddbed17bfaa49cb24d90/airv1schema.png)

#### 2. 创建产品和设备，配置规则引擎
请参考[设备互通](https://cloud.tencent.com/document/product/634/11913) 分别创建空调和门产品，再创建airConditioner1和door1设备，并配置好规则引擎。


## 二. 编译运行示例程序(以**密钥认证设备**为例)

#### 1. 编译 SDK
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
示例输出aircond_shadow_sample和door_mqtt_sample位于`output/release/bin`文件夹中

#### 2. 填写设备信息
将上面创建的airConditioner1设备的设备信息填写到一个JSON文件aircond_device_info.json中
```
{
    "auth_mode":"KEY",	
	"productId":"GYT9V6D4AF",
  	"deviceName":"airConditioner1",	
    "key_deviceinfo":{    
        "deviceSecret":"vXeds12qazsGsMyf5SMfs6OA6y"
    }
}
```
再将door1设备的设备信息填写到另一个JSON文件door_device_info.json中
```
{
    "auth_mode":"KEY",	
	"productId":"S3EUVBRJLB",
  	"deviceName":"door1",	
    "key_deviceinfo":{    
        "deviceSecret":"i92E3QMNmxi5hvIxUHjO8gTdg"
    }
}
```

#### 3. 执行aircond_shadow_sample示例程序
因为这个场景涉及到两个sample同时运行，可以先在当前终端console运行空调示例，可以看到示例订阅了 /{productID}/{deviceName}/control 主题，然后就处于循环等待状态
```
 ./output/release/bin/aircond_shadow_sample -c ./aircond_device_info.json
INF|2019-09-16 23:25:17|device.c|iot_device_info_set(67): SDK_Ver: 3.1.0, Product_ID: GYT9V6D4AF, Device_Name: airConditioner1
DBG|2019-09-16 23:25:17|HAL_TLS_mbedtls.c|HAL_TLS_Connect(204): Setting up the SSL/TLS structure...
DBG|2019-09-16 23:25:17|HAL_TLS_mbedtls.c|HAL_TLS_Connect(246): Performing the SSL/TLS handshake...
DBG|2019-09-16 23:25:17|HAL_TLS_mbedtls.c|HAL_TLS_Connect(247): Connecting to /GYT9V6D4AF.iotcloud.tencentdevices.com/8883...
INF|2019-09-16 23:25:18|HAL_TLS_mbedtls.c|HAL_TLS_Connect(269): connected with /GYT9V6D4AF.iotcloud.tencentdevices.com/8883...
INF|2019-09-16 23:25:19|mqtt_client.c|IOT_MQTT_Construct(125): mqtt connect with id: Nh9Vc success
DBG|2019-09-16 23:25:19|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=$shadow/operation/result/GYT9V6D4AF/airConditioner1|packet_id=56171
DBG|2019-09-16 23:25:19|shadow_client.c|_shadow_event_handler(63): shadow subscribe success, packet-id=56171
INF|2019-09-16 23:25:19|aircond_shadow_sample.c|event_handler(96): subscribe success, packet-id=56171
INF|2019-09-16 23:25:19|shadow_client.c|IOT_Shadow_Construct(172): Sync device data successfully
INF|2019-09-16 23:25:19|aircond_shadow_sample.c|main(256): Cloud Device Construct Success
DBG|2019-09-16 23:25:19|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=GYT9V6D4AF/airConditioner1/control|packet_id=56172
DBG|2019-09-16 23:25:19|shadow_client.c|_shadow_event_handler(63): shadow subscribe success, packet-id=56172
INF|2019-09-16 23:25:19|aircond_shadow_sample.c|event_handler(96): subscribe success, packet-id=56172
INF|2019-09-16 23:25:19|aircond_shadow_sample.c|main(291): airConditioner state: close
INF|2019-09-16 23:25:19|aircond_shadow_sample.c|main(292): currentTemperature: 32.000000, energyConsumption: 0.000000
```

#### 4. 执行door_mqtt_sample示例程序，模拟回家事件
这时打开另一个终端console，运行门的示例，可以看到示例往 /{productID}/{deviceName}/event 主题发送了JSON消息{"action": "come_home", "targetDevice": "airConditioner1"}，表示将回家的消息通知目标设备airConditioner1，然后程序就退出了

```
./output/release/bin/door_mqtt_sample -c ../devinfo/door_device_info.json -t airConditioner1 -a come_home
INF|2019-09-16 23:29:11|device.c|iot_device_info_set(67): SDK_Ver: 3.1.0, Product_ID: S3EUVBRJLB, Device_Name: test_dev_key
DBG|2019-09-16 23:29:11|HAL_TLS_mbedtls.c|HAL_TLS_Connect(204): Setting up the SSL/TLS structure...
DBG|2019-09-16 23:29:11|HAL_TLS_mbedtls.c|HAL_TLS_Connect(246): Performing the SSL/TLS handshake...
DBG|2019-09-16 23:29:11|HAL_TLS_mbedtls.c|HAL_TLS_Connect(247): Connecting to /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2019-09-16 23:29:11|HAL_TLS_mbedtls.c|HAL_TLS_Connect(269): connected with /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2019-09-16 23:29:11|mqtt_client.c|IOT_MQTT_Construct(125): mqtt connect with id: d89Wh success
INF|2019-09-16 23:29:11|door_mqtt_sample.c|main(229): Cloud Device Construct Success
DBG|2019-09-16 23:29:11|mqtt_client_publish.c|qcloud_iot_mqtt_publish(329): publish topic seq=46683|topicName=S3EUVBRJLB/test_dev_key/event|payload={"action": "come_home", "targetDevice": "airConditioner1"}
INF|2019-09-16 23:29:11|door_mqtt_sample.c|main(246): Wait for publish ack
INF|2019-09-16 23:29:11|door_mqtt_sample.c|event_handler(81): publish success, packet-id=46683
INF|2019-09-16 23:29:13|mqtt_client_connect.c|qcloud_iot_mqtt_disconnect(437): mqtt disconnect!
INF|2019-09-16 23:29:13|mqtt_client.c|IOT_MQTT_Destroy(186): mqtt release!
```

#### 5. 观察空调设备的消息接收，模拟消息响应
这个时候观察aircond_shadow_sample的打印输出，可以看到已经收到door1发送的回家消息，并将空调打开，开始调节温度。
```
INF|2019-09-16 23:29:11|aircond_shadow_sample.c|main(291): airConditioner state: close
INF|2019-09-16 23:29:11|aircond_shadow_sample.c|main(292): currentTemperature: 32.000000, energyConsumption: 0.000000
INF|2019-09-16 23:29:12|aircond_shadow_sample.c|on_message_callback(140): Receive Message With topicName:GYT9V6D4AF/airConditioner1/control, payload:{"action":"come_home","targetDevice":"airConditioner1"}
INF|2019-09-16 23:29:12|aircond_shadow_sample.c|main(291): airConditioner state: open
INF|2019-09-16 23:29:12|aircond_shadow_sample.c|main(292): currentTemperature: 31.000000, energyConsumption: 1.000000
INF|2019-09-16 23:29:13|aircond_shadow_sample.c|main(291): airConditioner state: open
INF|2019-09-16 23:29:13|aircond_shadow_sample.c|main(292): currentTemperature: 30.000000, energyConsumption: 2.000000
```

#### 6. 执行door_mqtt_sample示例程序，模拟离开事件
这时如果再次运行门的示例，更改程序启动参数，可以看到示例往 /{productID}/{deviceName}/event 主题发送了JSON消息{"action": "leave_home", "targetDevice": "airConditioner1"}，表示将离开的消息通知目标设备airConditioner1

```
./output/release/bin/door_mqtt_sample -c ../devinfo/door_device_info.json -t airConditioner1 -a leave_home
INF|2019-09-16 23:40:35|device.c|iot_device_info_set(67): SDK_Ver: 3.1.0, Product_ID: S3EUVBRJLB, Device_Name: test_dev_key
DBG|2019-09-16 23:40:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(204): Setting up the SSL/TLS structure...
DBG|2019-09-16 23:40:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(246): Performing the SSL/TLS handshake...
DBG|2019-09-16 23:40:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(247): Connecting to /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2019-09-16 23:40:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(269): connected with /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2019-09-16 23:40:35|mqtt_client.c|IOT_MQTT_Construct(125): mqtt connect with id: 3I59W success
INF|2019-09-16 23:40:35|door_mqtt_sample.c|main(229): Cloud Device Construct Success
DBG|2019-09-16 23:40:35|mqtt_client_publish.c|qcloud_iot_mqtt_publish(329): publish topic seq=39867|topicName=S3EUVBRJLB/test_dev_key/event|payload={"action": "leave_home", "targetDevice": "airConditioner1"}
INF|2019-09-16 23:40:35|door_mqtt_sample.c|main(246): Wait for publish ack
INF|2019-09-16 23:40:35|door_mqtt_sample.c|event_handler(81): publish success, packet-id=39867
INF|2019-09-16 23:40:37|mqtt_client_connect.c|qcloud_iot_mqtt_disconnect(437): mqtt disconnect!
INF|2019-09-16 23:40:37|mqtt_client.c|IOT_MQTT_Destroy(186): mqtt release!
```

#### 7. 观察空调设备的消息接收，模拟消息响应
这个时候观察aircond_shadow_sample的打印输出，可以看到已经收到door1发送的离开消息，并将空调关闭，温度开始升高。
```
INF|2019-09-16 23:40:35|aircond_shadow_sample.c|main(291): airConditioner state: open
INF|2019-09-16 23:40:35|aircond_shadow_sample.c|main(292): currentTemperature: 26.000000, energyConsumption: 6.000000
INF|2019-09-16 23:40:36|aircond_shadow_sample.c|on_message_callback(140): Receive Message With topicName:GYT9V6D4AF/airConditioner1/control, payload:{"action":"leave_home","targetDevice":"airConditioner1"}
INF|2019-09-16 23:40:36|aircond_shadow_sample.c|main(291): airConditioner state: close
INF|2019-09-16 23:40:36|aircond_shadow_sample.c|main(292): currentTemperature: 26.500000, energyConsumption: 6.000000
INF|2019-09-16 23:40:37|aircond_shadow_sample.c|main(291): airConditioner state: close
INF|2019-09-16 23:40:37|aircond_shadow_sample.c|main(292): currentTemperature: 27.000000, energyConsumption: 6.000000
INF|2019-09-16 23:40:39|aircond_shadow_sample.c|main(291): airConditioner state: close
```

至此，完成了模拟设备互通联动的示例运行。
