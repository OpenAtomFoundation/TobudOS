## 概述

腾讯云物联网平台为每个创建的产品分配唯一标识 ProductID，用户可以自定义 DeviceName 标识设备，用产品标识 + 设备标识 + 设备证书/密钥来验证设备的合法性。用户在创建产品时需要选择设备认证方式，在设备接入时需要根据指定的方式上报产品、设备信息与对应的密钥信息，认证通过后才能连接物联网通信平台。由于不同用户的设备端资源、安全等级要求都不同，平台提供了多种认证方案，以满足不同的使用场景。

物联网平台提供以下三种认证方案：
- 证书认证（设备级）：为每台设备分配证书 + 私钥，使用非对称加密认证接入，用户需要为每台设备烧录不同的配置信息。
- 密钥认证（设备级）：为每台设备分配设备密钥，使用对称加密认证接入，用户需要为每台设备烧录不同的配置信息。
- 动态注册认证（产品级）：为同一产品下的所有设备分配统一密钥，设备通过注册请求获取设备证书/密钥后认证接入，用户可以为同一批设备烧录相同的配置信息。IoT Explorer平台暂不支持动态注册认证。

三种方案在易用性、安全性和对设备资源要求上各有优劣，您可以根据自己的业务场景综合评估选择。方案对比如下：

| 特性 | 证书认证 |密钥认证| 动态注册认证|
|--------|--------|--------|--------|
|  设备烧录信息    | ProductId、DeviceName、<br>设备证书、设备私钥| ProductId、DeviceName、设备密钥      |ProductId、DeviceName、ProductSecret|
|  是否需要提前创建设备     |   必须|  必须     |支持根据注册请求中携带的 DeviceName 自动创建|
|  安全性     |     高 |  一般     | 一般 |
|  使用限制      |    单产品下最多创建20万设备|   单产品下最多创建20万设备     | 单产品下最多创建20万设备，用户可自定义通过注册请求自动创建的设备数上限|
|  设备资源要求     |    较高，需要支持 TLS|  较低     |较低，支持 AES 即可|

## 设备身份信息
 - 证书设备要通过平台的安全认证，必须具备四元组信息：产品 ID（ProductId）、设备名（DeviceName）、设备证书文件（DeviceCert）、设备私钥文件（DevicePrivateKey），其中证书文件和私钥文件由平台生成，且一一对应。
 - 密钥设备要通过平台的安全认证，必须具备三元组信息：产品 ID（ProductId）、设备名（DeviceName）、设备密钥（DeviceSecret），其中设备密钥由平台生成。

## 设备身份信息烧录
- 创建产品后，在物联网开发平台 [控制台](https://console.cloud.tencent.com/iotexplorer) 或者通过 [云 API](https://cloud.tencent.com/document/product/1081/37178) 创建设备，并获取对应的设备信息，将上述的四元组或者三元组信息，在设备生产的特定环节，烧录到非易失介质中，设备 SDK 运行时读取存放的设备信息，进行设备认证。

- SDK提供了设备信息读写的HAL接口，必须实现。可以参考linux平台HAL_Device_linux.c中设备信息读写的实现。

- 设备信息HAL接口 ：

| HAL_API                            | 说明                                 |
| -----------------------------------| ----------------------------------  |
| HAL_SetDevInfo                  	| 设备信息写入    |
| HAL_GetDevInfo                   	| 设备信息读取    |

## 开发阶段设备信息配置

创建设备之后，需要将设备信息（ProductID/DeviceName/DeviceSecret/Cert/Key文件）配置在SDK中才能正确运行示例。在开发阶段，SDK 提供两种方式存储设备信息：
1. 存放在代码中（编译选项DEBUG_DEV_INFO_USED = ON），则在`platform/os/xxx/HAL_Device_xxx.c`中修改设备信息，在无文件系统的平台下可以使用这种方式。
```
/* product Id  */
static char sg_product_id[MAX_SIZE_OF_PRODUCT_ID + 1]	 = "PRODUCT_ID";

/* device name */
static char sg_device_name[MAX_SIZE_OF_DEVICE_NAME + 1]  = "YOUR_DEV_NAME";

#ifdef DEV_DYN_REG_ENABLED
/* product secret for device dynamic Registration  */
static char sg_product_secret[MAX_SIZE_OF_PRODUCT_SECRET + 1]  = "YOUR_PRODUCT_SECRET";
#endif

#ifdef AUTH_MODE_CERT
/* public cert file name of certificate device */
static char sg_device_cert_file_name[MAX_SIZE_OF_DEVICE_CERT_FILE_NAME + 1]      = "YOUR_DEVICE_NAME_cert.crt";
/* private key file name of certificate device */
static char sg_device_privatekey_file_name[MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME + 1] = "YOUR_DEVICE_NAME_private.key";
#else
/* device secret of PSK device */
static char sg_device_secret[MAX_SIZE_OF_DEVICE_SECRET + 1] = "YOUR_IOT_PSK";
#endif
```

2. 存放在配置文件中（编译选项DEBUG_DEV_INFO_USED = OFF），则在`device_info.json`文件修改设备信息，此方式下更改设备信息不需重新编译SDK，在Linux/Windows平台下开发推荐使用这种方式。
```
{
    "auth_mode":"KEY/CERT",

    "productId":"PRODUCT_ID",
    "productSecret":"YOUR_PRODUCT_SECRET",
    "deviceName":"YOUR_DEV_NAME",

    "key_deviceinfo":{    
        "deviceSecret":"YOUR_IOT_PSK"
    },

    "cert_deviceinfo":{
        "devCertFile":"YOUR_DEVICE_CERT_FILE_NAME",
        "devPrivateKeyFile":"YOUR_DEVICE_PRIVATE_KEY_FILE_NAME"
    },

    "subDev":{
        "sub_productId":"YOUR_SUBDEV_PRODUCT_ID",
        "sub_devName":"YOUR_SUBDEV_DEVICE_NAME"
    }
}
```

##  应用示例
-  初始化连接参数

```
static DeviceInfo sg_devInfo;

static int _setup_connect_init_params(MQTTInitParams* initParams)
{
	int ret;
	
	ret = HAL_GetDevInfo((void *)&sg_devInfo);	
	if(QCLOUD_RET_SUCCESS != ret){
		return ret;
	}
		
	initParams->device_name = sg_devInfo.device_name;
	initParams->product_id = sg_devInfo.product_id;
	 ......
}	
```


-  密钥设备鉴权参数生成

```
static int _serialize_connect_packet(unsigned char *buf, size_t buf_len, MQTTConnectParams *options, uint32_t *serialized_len) {
			......
			......
    int username_len = strlen(options->client_id) + strlen(QCLOUD_IOT_DEVICE_SDK_APPID) + MAX_CONN_ID_LEN + cur_timesec_len + 4;
    options->username = (char*)HAL_Malloc(username_len);
    get_next_conn_id(options->conn_id);
	HAL_Snprintf(options->username, username_len, "%s;%s;%s;%ld", options->client_id, QCLOUD_IOT_DEVICE_SDK_APPID, options->conn_id, cur_timesec);

#if defined(AUTH_WITH_NOTLS) && defined(AUTH_MODE_KEY)
     if (options->device_secret != NULL && options->username != NULL) {
    	 char                sign[41]   = {0};
    	 utils_hmac_sha1(options->username, strlen(options->username), sign, options->device_secret, options->device_secret_len);
    	 options->password = (char*) HAL_Malloc (51);
    	 if (options->password == NULL) IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
		 HAL_Snprintf(options->password, 51, "%s;hmacsha1", sign);
     }
#endif
			......
}
```

