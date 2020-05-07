# 设备接入及动态注册

关于IoT Hub设备接入认证的详细介绍，请参考 [设备身份认证 ](https://cloud.tencent.com/document/product/634/35272)
简单来说，物联网通信平台IoT Hub提供三种设备端接入认证的方案：

- 证书认证（设备级）：为每台设备分配证书 + 私钥，使用非对称加密认证接入，用户需要为每台设备烧录不同的配置信息。
- 密钥认证（设备级）：为每台设备分配设备密钥，使用对称加密认证接入，用户需要为每台设备烧录不同的配置信息。
- 动态注册认证（产品级）：为同一产品下的所有设备分配统一密钥，设备通过注册请求获取设备证书/密钥后认证接入，用户可以为同一批设备烧录相同的配置信息。

使用设备动态注册功能，可以使得同一个产品型号的设备出厂时烧录的是统一的固件，该固件只包含统一的产品ID和产品密钥。设备出厂后判断设备信息为空（判断设备信息为空的逻辑由业务逻辑实现，可参考示例），则触发设备动态注册，从平台申请设备的证书（创建的产品为证书认证产品）或者设备的密钥（创建的产品为密钥认证方式），将获取到的设备信息保存下来之后，后续的设备接入就可以使用。

## 一. 控制台使能动态注册

动态注册时，设备名称的生成有两种方式，一种是在控制台使能了动态注册产品的自动创建设备功能，则设备可以自行生成设备名称，但需保证同一产品下没有重复，一般取设备的唯一信息比如CPUID或mac地址。另一种是没有使能动态注册产品的自动创建设备，则需要在控制台预先录入各设备的名称，且设备动态注册时设备要与录入的设备名称一致，此种方式更加安全，便利性有所下降。

控制台使能动态注册设置如下图示：
![](https://main.qcloudimg.com/raw/a02f57cbe40f26ead94170396d78253c.jpg)

将产品密钥ProductSecret保存下来。

## 二. 编译运行示例程序(以**密钥认证设备**为例)
#### 1. 编译 SDK
修改CMakeLists.txt确保以下选项存在
```
set(BUILD_TYPE                   "release")
set(COMPILE_TOOLS                "gcc") 
set(PLATFORM 	                "linux")
set(FEATURE_MQTT_COMM_ENABLED ON)
set(FEATURE_DEV_DYN_REG_ENABLED ON)
set(FEATURE_AUTH_MODE "KEY")
set(FEATURE_AUTH_WITH_NOTLS OFF)
set(FEATURE_DEBUG_DEV_INFO_USED  OFF)
```
执行脚本编译
```
./cmake_build.sh 
```
示例输出dynreg_dev_sample位于`output/release/bin`文件夹中

#### 2. 填写设备信息
将控制台获取到的产品信息填写到一个JSON文件dynreg_device_info.json中，其中deviceName字段填写要生成的设备名字，deviceSecret字段保持为"YOUR_IOT_PSK"，这样dynreg_dev_sample就会判断设备信息为空，知道这个设备是需要进行动态注册。这部分逻辑可以由用户自行实现，sample仅作示例。
```
{
    "auth_mode":"KEY",
    "productId":"S3EUVBRJLB",
    "productSecret":"8Xz56tyfgQAZEDCTUGau4snA",
    "deviceName":"device_1234",
    "key_deviceinfo":{
        "deviceSecret":"YOUR_IOT_PSK"
    }
}
```
#### 3. 运行示例
执行设备动态注册例程dynreg_dev_sample：
```
./output/release/bin/dynreg_dev_sample -c ./dynreg_device_info.json 
DBG|2019-09-17 11:50:35|dynreg_dev_sample.c|main(80): dev psk not exist!
DBG|2019-09-17 11:50:35|dynreg.c|IOT_DynReg_Device(467): sign:ZWM4YTEyMWE2ODUxYzk1M2Q0MDc2OWNmN2FhMTg1ZWM1ODgxMWNkNQ==
DBG|2019-09-17 11:50:35|dynreg.c|IOT_DynReg_Device(483): request:{"deviceName":"device_1234","nonce":1439928322,"productId":"S3EUVBRJLB","timestamp":1568692235,"signature":"ZWM4YTEyMWE2ODUxYzk1M2Q0MDc2OWNmN2FhMTg1ZWM1ODgxMWNkNQ=="}
DBG|2019-09-17 11:50:35|dynreg.c|IOT_DynReg_Device(485): resbuff len:256
DBG|2019-09-17 11:50:35|HAL_TLS_mbedtls.c|_mbedtls_client_init(134): psk/pskid is empty!|psk=(null)|psd_id=(null)
DBG|2019-09-17 11:50:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(204): Setting up the SSL/TLS structure...
DBG|2019-09-17 11:50:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(246): Performing the SSL/TLS handshake...
DBG|2019-09-17 11:50:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(247): Connecting to /gateway.tencentdevices.com/443...
INF|2019-09-17 11:50:35|HAL_TLS_mbedtls.c|HAL_TLS_Connect(269): connected with /gateway.tencentdevices.com/443...
DBG|2019-09-17 11:50:35|utils_httpc.c|qcloud_http_client_connect(749): http client connect success
DBG|2019-09-17 11:50:37|dynreg.c|_parse_devinfo(244): recv: {"code":0,"message":"","len":53,"payload":"eHGRUBar9LBPI+mcaHnVEZsj05mSdoZLPhd54hDJXv/2va2rXSpfddgRy5XE/FIS835NjUr5Mhw1AJSg4yGC/w=="}
DBG|2019-09-17 11:50:37|dynreg.c|_parse_devinfo(258): payload:eHGRUBar9LBPI+mcaHnVEZsj05mSdoZLPhd54hDJXv/2va2rXSpfddgRy5XE/FIS835NjUr5Mhw1AJSg4yGC/w==
DBG|2019-09-17 11:50:37|dynreg.c|IOT_DynReg_Device(489): request dev info success
DBG|2019-09-17 11:50:37|HAL_Device_linux.c|iot_save_devinfo_to_json_file(340): JsonDoc(183):{
"auth_mode":"KEY",
"productId":"S3EUVBRJLB",
"productSecret":"8XzjN8rfgFVGDTNTUGau4snA",
"deviceName":"device_1234",
"key_deviceinfo":{
"deviceSecret":"7WmFArtyFGH5632QwJWtYwio"
}
}
DBG|2019-09-17 11:50:37|dynreg_dev_sample.c|main(99): dynamic register success,productID: S3EUVBRJLB, devName: device_1234, device_secret: 7WmFArtyFGH5632QwJWtYwio
```
可以看到设备动态注册已经成功，获取到的设备密钥也写入到dynreg_device_info.json文件中了
