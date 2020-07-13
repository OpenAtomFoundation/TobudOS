# OTA设备固件升级

关于物联平台 OTA 固件升级功能及流程介绍，请参考官网文档[设备固件升级](https://cloud.tencent.com/document/product/634/14674)

## 一. 上传固件

进行固件升级，首先要上传固件到物联网后台，可以通过控制台进行上传，如下图示：
![](https://main.qcloudimg.com/raw/219ab155f886e2ac1a1ebd0d7a1184aa.png)

## 二. 编译运行示例程序(以**MQTT密钥认证设备**为例)
#### 1. 编译 SDK
修改CMakeLists.txt确保以下选项存在
```
set(BUILD_TYPE                   "release")
set(COMPILE_TOOLS                "gcc") 
set(PLATFORM 	                "linux")
set(FEATURE_MQTT_COMM_ENABLED ON)
set(FEATURE_OTA_COMM_ENABLED ON)
set(FEATURE_OTA_SIGNAL_CHANNEL "MQTT")
set(FEATURE_AUTH_MODE "KEY")
set(FEATURE_AUTH_WITH_NOTLS OFF)
set(FEATURE_DEBUG_DEV_INFO_USED  OFF)
```
执行脚本编译
```
./cmake_build.sh 
```
示例输出ota_mqtt_sample位于`output/release/bin`文件夹中

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

#### 3. 运行示例
执行OTA MQTT例程ota_mqtt_sample：
```
./output/release/bin/ota_mqtt_sample 
INF|2020-03-04 16:50:35|qcloud_iot_device.c|iot_device_info_set(50): SDK_Ver: 3.1.2, Product_ID: S3EUVBRJLB, Device_Name: test_device
DBG|2020-03-04 16:50:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(206): Setting up the SSL/TLS structure...
DBG|2020-03-04 16:50:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(248): Performing the SSL/TLS handshake...
DBG|2020-03-04 16:50:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(249): Connecting to /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2020-03-04 16:50:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(271): connected with /S3EUVBRJLB.iotcloud.tencentdevices.com/8883...
INF|2020-03-04 16:50:35|mqtt_client.c|IOT_MQTT_Construct(111): mqtt connect with id: BzaMF success
INF|2020-03-04 16:50:35|ota_mqtt_sample.c|main(516): Cloud Device Construct Success
DBG|2020-03-04 16:50:35|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(141): topicName=$ota/update/S3EUVBRJLB/test_device|packet_id=63333
INF|2020-03-04 16:50:35|ota_mqtt_sample.c|_event_handler(77): subscribe success, packet-id=63333
DBG|2020-03-04 16:50:35|ota_mqtt.c|_otamqtt_event_callback(117): OTA topic subscribe success
INF|2020-03-04 16:50:36|ota_mqtt_sample.c|_get_local_fw_running_version(363): FW running version: 1.0.0
DBG|2020-03-04 16:50:36|mqtt_client_publish.c|qcloud_iot_mqtt_publish(334): publish topic seq=63334|topicName=$ota/report/S3EUVBRJLB/test_device|payload={"type": "report_version", "report":{"version":"1.0.0"}}
INF|2020-03-04 16:50:36|ota_mqtt_sample.c|_event_handler(89): publish success, packet-id=63334
DBG|2020-03-04 16:50:36|ota_mqtt.c|_otamqtt_upgrage_cb(103): topic=$ota/update/S3EUVBRJLB/test_device
INF|2020-03-04 16:50:36|ota_mqtt.c|_otamqtt_upgrage_cb(104): len=86, topic_msg={"result_code":0,"result_msg":"success","type":"report_version_rsp","version":"1.0.0"}
INF|2020-03-04 16:50:36|ota_client.c|_ota_callback(102): Report version success!
INF|2020-03-04 16:50:36|ota_mqtt_sample.c|process_ota(389): wait for ota upgrade command...
INF|2020-03-04 16:50:37|ota_mqtt_sample.c|process_ota(389): wait for ota upgrade command...
```
可以看到设备已经上报了当前的固件版本，并在等待固件升级的命令下发

#### 4. 执行固件升级命令

在收到设备上报的固件版本之后，可以选择要升级的新固件版本并执行升级命令，可以通过控制台进行操作，如下图示：
![](https://main.qcloudimg.com/raw/ac71788c5bc5d542eb10292751e12fbe.png)

这个时候再观察sample的示例打印，可以看到设备端通过MQTT接收到升级命令，并且通过HTTPS去下载固件并保存到本地
```
INF|2020-03-04 16:50:38|ota_mqtt_sample.c|process_ota(389): wait for ota upgrade command...
DBG|2020-03-04 16:50:39|ota_mqtt.c|_otamqtt_upgrage_cb(103): topic=$ota/update/S3EUVBRJLB/test_device
INF|2020-03-04 16:50:39|ota_mqtt.c|_otamqtt_upgrage_cb(104): len=454, topic_msg={"file_size":175436,"md5sum":"ad4615b866c13afb8b293a679bfa5dc4","type":"update_firmware","url":"https://ota-1255858890.cos.ap-guangzhou.myqcloud.com/100008768029_S3EUVBRJLB_1.3.0?sign=q-sign-algorithm%3Dsha1%26q-ak%3DAKIDdO8ldrUa0Uts4H5Gzx6FZ9nfedjpiCd7%26q-sign-time%3D1583311839%3B1583398239%26q-key-time%3D1583311839%3B1583398239%26q-header-list%3D%26q-url-param-list%3D%26q-signature%3D1215yt303628f88f31c9a7734ead8a7fa32ed1b3%00","version":"1.3.0"}

INF|2020-03-04 16:50:39|ota_mqtt_sample.c|process_ota(389): wait for ota upgrade command...
ERR|2020-03-04 16:50:39|ota_mqtt_sample.c|_get_local_fw_info(251): open file ./FW_S3EUVBRJLBtest_device.json failed
DBG|2020-03-04 16:50:39|ota_client.c|IOT_OTA_StartDownload(347): to download FW from offset: 0, size: 175436
DBG|2020-03-04 16:50:39|ota_fetch.c|ofc_Init(83): head_content:Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Encoding: gzip, deflate
Range: bytes=0-175436

DBG|2020-03-04 16:50:39|HAL_TLS_mbedtls.c|_mbedtls_client_init(134): psk/pskid is empty!|psk=(null)|psd_id=(null)
DBG|2020-03-04 16:50:39|HAL_TLS_mbedtls.c|HAL_TLS_Connect(206): Setting up the SSL/TLS structure...
DBG|2020-03-04 16:50:39|HAL_TLS_mbedtls.c|HAL_TLS_Connect(248): Performing the SSL/TLS handshake...
DBG|2020-03-04 16:50:39|HAL_TLS_mbedtls.c|HAL_TLS_Connect(249): Connecting to /ota-1255858890.cos.ap-guangzhou.myqcloud.com/443...
INF|2020-03-04 16:50:40|HAL_TLS_mbedtls.c|HAL_TLS_Connect(271): connected with /ota-1255858890.cos.ap-guangzhou.myqcloud.com/443...
DBG|2020-03-04 16:50:40|utils_httpc.c|qcloud_http_client_connect(747): http client connect success
DBG|2020-03-04 16:50:40|mqtt_client_publish.c|qcloud_iot_mqtt_publish(340): publish packetID=0|topicName=$ota/report/S3EUVBRJLB/test_device|payload={"type": "report_progress", "report": {"progress": {"state":"downloading", "percent":"0", "result_code":"0", "result_msg":""}, "version": "1.3.0"}}
DBG|2020-03-04 16:50:41|mqtt_client_publish.c|qcloud_iot_mqtt_publish(340): publish packetID=0|topicName=$ota/report/S3EUVBRJLB/test_device|payload={"type": "report_progress", "report": {"progress": {"state":"downloading", "percent":"31", "result_code":"0", "result_msg":""}, "version": "1.3.0"}}
DBG|2020-03-04 16:50:42|mqtt_client_publish.c|qcloud_iot_mqtt_publish(340): publish packetID=0|topicName=$ota/report/S3EUVBRJLB/test_device|payload={"type": "report_progress", "report": {"progress": {"state":"downloading", "percent":"59", "result_code":"0", "result_msg":""}, "version": "1.3.0"}}
DBG|2020-03-04 16:50:43|mqtt_client_publish.c|qcloud_iot_mqtt_publish(340): publish packetID=0|topicName=$ota/report/S3EUVBRJLB/test_device|payload={"type": "report_progress", "report": {"progress": {"state":"downloading", "percent":"88", "result_code":"0", "result_msg":""}, "version": "1.3.0"}}
DBG|2020-03-04 16:50:43|mqtt_client_publish.c|qcloud_iot_mqtt_publish(340): publish packetID=0|topicName=$ota/report/S3EUVBRJLB/test_device|payload={"type": "report_progress", "report": {"progress": {"state":"downloading", "percent":"100", "result_code":"0", "result_msg":""}, "version": "1.3.0"}}
INF|2020-03-04 16:50:43|ota_client.c|IOT_OTA_Ioctl(638): FW MD5 check: origin=ad4615b866c13afb8b293a679bfa5dc4, now=ad4615b866c13afb8b293a679bfa5dc4
INF|2020-03-04 16:50:43|ota_mqtt_sample.c|process_ota(456): The firmware is valid
DBG|2020-03-04 16:50:43|mqtt_client_publish.c|qcloud_iot_mqtt_publish(334): publish topic seq=63335|topicName=$ota/report/S3EUVBRJLB/test_device|payload={"type": "report_progress", "report":{"progress":{"state":"done", "result_code":"0", "result_msg":""}, "version":"1.3.0"}}
INF|2020-03-04 16:50:44|ota_mqtt_sample.c|_event_handler(89): publish success, packet-id=63335
INF|2020-03-04 16:50:44|mqtt_client_connect.c|qcloud_iot_mqtt_disconnect(450): mqtt disconnect!
INF|2020-03-04 16:50:44|ota_mqtt.c|_otamqtt_event_callback(135): mqtt client has been destroyed
INF|2020-03-04 16:50:44|mqtt_client.c|IOT_MQTT_Destroy(171): mqtt release!
```

## 三. 代码使用说明

1. 固件升级，支持通过HTTP或者HTTPS来下载固件，可以通过编译选项FEATURE_OTA_USE_HTTPS来选择
2. SDK的固件升级，支持断点续传功能，即当固件下载过程因为网络或者其他原因中断了，则再次下载的时候，可以从上一次中断的位置继续下载。
3. 在ota_mqtt_sample.c中，process_ota函数为OTA的核心流程，包括了版本上报，等待升级命令，发起下载并保存，校验文件MD5以及回复升级结果的过程，同时为断点续传功能实时保存下载状态。该函数在各个平台适配的时候可以保持不变。而跟固件保存，断点续传状态保存相关的功能函数，则跟平台密切相关，示例中使用了文件操作，在支持标准文件操作的系统比如Linux/Windows上面可以直接使用，对于其他平台则需要进行适配。


