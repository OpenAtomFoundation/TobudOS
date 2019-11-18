# 物联网开发平台快速入门

物联网开发平台**IoT Explorer**是基于 MQTT 基础能力上，对数据协议的进一步抽象封装，形成[数据模板协议](https://cloud.tencent.com/document/product/1081/34916?!preview&!editLang=zh)。平台基于数据模板的协议，提供丰富的数据展示、调试及应用开发等技术资源，将应用端和设备的开发模板化，加速产品开发进程。

本文档将讲述如何在物联网开发平台控制台申请设备，创建模板，并结合 C-SDK 快速体验设备基于数据模板的数据收发、事件上报及设备调试。

## 一. 控制台创建设备

#### 1. 注册/登录腾讯云账号
访问[腾讯云登录页面](https://cloud.tencent.com/login?s_url=https%3A%2F%2Fcloud.tencent.com%2F), 点击[立即注册](https://cloud.tencent.com/register?s_url=https%3A%2F%2Fcloud.tencent.com%2F), 免费获取腾讯云账号，若您已有账号，可直接登录。

#### 2. 访问物联网开发控制台
登录后点击右上角控制台，进入控制台后，鼠标悬停在云产品上，弹出层叠菜单，点击物联网开发平台。

或者直接访问[物联网开发平台控制台](https://console.cloud.tencent.com/iotexplorer)

#### 3. 创建产品和设备
3.1 创建项目
![](https://main.qcloudimg.com/raw/50dd24cfa44fbb063d4337eb94ae5d0f.jpg)

3.2.创建并选择和产品比较相近的模板产品，参阅[产品定义](https://cloud.tencent.com/document/product/1081/34739?!preview&!editLang=zh)。
![](https://main.qcloudimg.com/raw/c8bd19dee90765762bd6c1f98bd0dd2c.jpg)
![](https://main.qcloudimg.com/raw/4b9fe8f5df7a20ebbab1f0e3693ded8a.jpg)

3.3 定义产品的数据和事件模板，参阅[数据模板创建](https://cloud.tencent.com/document/product/1081/34739?!preview&!editLang=zh#.E6.95.B0.E6.8D.AE.E6.A8.A1.E6.9D.BF)，数据模板的说明参见[数据模板协议](https://cloud.tencent.com/document/product/1081/34916?!preview&!editLang=zh)。
![](https://main.qcloudimg.com/raw/17ef8daac4da6f9775ea02bddf988ca2.jpg)

3.4 完成产品创建和数据模板定义后，创建设备，则每一个创建的设备都具备这个产品下的数据模板属性，如下图示。
![](https://main.qcloudimg.com/raw/937698b945dfd2c4f34bffd40ff5165d.jpg)

3.5 查询产品和设备信息
![](https://main.qcloudimg.com/raw/13972a011bb5382d00d73545534af91a.jpg)
![](https://main.qcloudimg.com/raw/17ef8daac4da6f9775ea02bddf988ca2.jpg)

3.6 导出数据模板json文件，并通过脚本工具生成数据模板的配置文件
![](https://main.qcloudimg.com/raw/0951d7c3f540ca716442e08651a0efa5.jpg)

将下载的json文件拷贝到tools目录，执行./codegen.py -c xx/config.json -d  ../targetdir/ 命令,则会根据json文件在target目录生成所定义产品的数据模板及事件的配置文件：
```
./codegen.py -c light.json -d ../samples/data_template/
加载 light.json 文件成功
文件 ../samples/data_template/data_config.c 生成成功
文件 ../samples/data_template/events_config.c 生成成功
```

## 二. 编译示例程序

详细的编译方式请参考**编译及配置选项说明文档**，本文基于 Linux 环境介绍示例编译运行(以**密钥认证设备**为例)。

#### 1. 填入设备信息
将上面步骤在腾讯云物联网平台创建的设备的设备信息填写到device_info.json中，如：
```
"auth_mode":"KEY",	
"productId":"5S6AQZ6IDE",
"deviceName":"dev01",	
"key_deviceinfo":{    
    "deviceSecret":"vX6PQqazsGsMyf5SMfs6OA6y"
}
```
#### 2. 基于数据模板的业务逻辑开发
数据模板示例data_template_sample.c，已实现数据、事件收发及响应的通用处理逻辑。但是具体的数据处理的业务逻辑需要用户自己根据业务逻辑添加，上下行业务逻辑添加的入口函数分别为deal_up_stream_user_logic 、deal_down_stream_user_logic，如果有字符串或json类型的数据模板，用户需要在函数update_self_define_value中完成数据的解析（其他数据类型会根据模板定义自动更新），可以参考基于场景的示例light_data_template_sample.c添加业务处理逻辑。

下行业务逻辑实现：
```
/* user's own down-stream code */
void deal_down_stream_user_logic(ProductDataDefine   * pData)
{
	Log_d("someting about your own product logic wait to be done");
}
```

上行业务逻辑实现：
```
/* demo for up-stream code */
int deal_up_stream_user_logic(DeviceProperty *pReportDataList[], int *pCount)
{
	int i, j;
	
     for (i = 0, j = 0; i < TOTAL_PROPERTY_COUNT; i++) {       
        if(eCHANGED == sg_DataTemplate[i].state) {
            pReportDataList[j++] = &(sg_DataTemplate[i].data_property);
			sg_DataTemplate[i].state = eNOCHANGE;
        }
    }
	*pCount = j;

	return (*pCount > 0)?QCLOUD_RET_SUCCESS:QCLOUD_ERR_FAILURE;
}
```

#### 3. 编译 SDK

首先修改CMakeLists.txt确保以下选项存在
```
set(BUILD_TYPE                   "release")
set(COMPILE_TOOLS                "gcc") 
set(PLATFORM 	                "linux")
set(FEATURE_MQTT_COMM_ENABLED ON)
set(FEATURE_AUTH_MODE "KEY")
set(FEATURE_AUTH_WITH_NOTLS OFF)
set(FEATURE_DEBUG_DEV_INFO_USED  OFF)  
```
完整编译库和示例
```
./cmake_build.sh 
```
输出的库文件，头文件及示例在`output/release`文件夹中。
在一次完整编译之后，若只需要编译示例，则执行
```
./cmake_build.sh samples
```

## 三. 执行示例程序

#### 执行 data_template_sample 示例程序

数据模板示例实现通用的数据模板和事件处理的框架，数据上下行和事件上报的日志如下：
```
 ./output/release/bin/data_template_sample 
INF|2019-09-18 15:11:54|device.c|iot_device_info_set(67): SDK_Ver: 3.1.0, Product_ID: 5S6AQZ6IDE, Device_Name: dev01
DBG|2019-09-18 15:11:54|HAL_TLS_mbedtls.c|HAL_TLS_Connect(204): Setting up the SSL/TLS structure...
DBG|2019-09-18 15:11:54|HAL_TLS_mbedtls.c|HAL_TLS_Connect(246): Performing the SSL/TLS handshake...
DBG|2019-09-18 15:11:54|HAL_TLS_mbedtls.c|HAL_TLS_Connect(247): Connecting to /5S6AQZ6IDE.wx-mqtt.tencentdevices.com/8883...
INF|2019-09-18 15:11:55|HAL_TLS_mbedtls.c|HAL_TLS_Connect(269): connected with /5S6AQZ6IDE.wx-mqtt.tencentdevices.com/8883...
INF|2019-09-18 15:11:55|mqtt_client.c|IOT_MQTT_Construct(125): mqtt connect with id: r7891 success
DBG|2019-09-18 15:11:55|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=$template/operation/result/5S6AQZ6IDE/dev01|packet_id=4412
DBG|2019-09-18 15:11:55|shadow_client.c|_shadow_event_handler(63): shadow subscribe success, packet-id=4412
INF|2019-09-18 15:11:55|data_template_sample.c|event_handler(124): subscribe success, packet-id=4412
INF|2019-09-18 15:11:55|shadow_client.c|IOT_Shadow_Construct(172): Sync device data successfully
INF|2019-09-18 15:11:55|data_template_sample.c|main(319): Cloud Device Construct Success
DBG|2019-09-18 15:11:55|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=$thing/down/event/5S6AQZ6IDE/dev01|packet_id=4413
INF|2019-09-18 15:11:55|data_template_sample.c|_register_data_template_property(270): data template property=time registered.
INF|2019-09-18 15:11:55|data_template_sample.c|_register_data_template_property(270): data template property=float registered.
INF|2019-09-18 15:11:55|data_template_sample.c|_register_data_template_property(270): data template property=light_switch registered.
INF|2019-09-18 15:11:55|data_template_sample.c|_register_data_template_property(270): data template property=color registered.
INF|2019-09-18 15:11:55|data_template_sample.c|_register_data_template_property(270): data template property=brightness registered.
INF|2019-09-18 15:11:55|data_template_sample.c|_register_data_template_property(270): data template property=name registered.
INF|2019-09-18 15:11:55|data_template_sample.c|main(340): Register data template propertys Success
DBG|2019-09-18 15:11:55|shadow_client.c|IOT_Shadow_Get(383): GET Request Document: {"clientToken":"explorer-5S6AQZ6IDE-0"}
DBG|2019-09-18 15:11:55|mqtt_client_publish.c|qcloud_iot_mqtt_publish(329): publish topic seq=4414|topicName=$template/operation/5S6AQZ6IDE/dev01|payload={"type":"get", "clientToken":"explorer-5S6AQZ6IDE-0"}
INF|2019-09-18 15:11:55|data_template_sample.c|event_handler(138): publish success, packet-id=4414
DBG|2019-09-18 15:11:55|shadow_client.c|_shadow_event_handler(63): shadow subscribe success, packet-id=4413
INF|2019-09-18 15:11:55|data_template_sample.c|event_handler(124): subscribe success, packet-id=4413
DBG|2019-09-18 15:11:55|shadow_client_manager.c|_on_operation_result_handler(278): type:get
DBG|2019-09-18 15:11:55|shadow_client.c|_update_ack_cb(114): requestAck=0
DBG|2019-09-18 15:11:55|shadow_client.c|_update_ack_cb(117): Received Json Document={"clientToken":"explorer-5S6AQZ6IDE-0","payload":{"state":{"reported":{"battery":35,"brightness":55,"color":0,"id":"19","name":"dev01","power_switch":0}},"timestamp":1568776522035,"version":401},"result":0,"timestamp":1568790715,"type":"get"}
DBG|2019-09-18 15:11:56|data_template_sample.c|main(389): No delta msg received...
DBG|2019-09-18 15:11:56|data_template_sample.c|main(409): no data need to be reported
DBG|2019-09-18 15:11:56|mqtt_client_publish.c|qcloud_iot_mqtt_publish(337): publish packetID=0|topicName=$thing/up/event/5S6AQZ6IDE/dev01|payload={"method":"events_post", "clientToken":"5S6AQZ6IDE-1", "events":[{"eventId":"all_function", "type":"alert", "timestamp":1568790716000, "params":{"bool":0,"int":0,"str":"","float":0.000000,"enum1":0,"time":0}},{"eventId":"status_report", "type":"info", "timestamp":1568790716000, "params":{"status":0,"message":""}},{"eventId":"hardware_fault", "type":"fault", "timestamp":1568790716000, "params":{"name":"","error_code":0}}]}
INF|2019-09-18 15:11:59|qcloud_iot_event.c|_on_event_reply_callback(96): Receive Message With topicName:$thing/down/event/5S6AQZ6IDE/dev01, payload:{"method":"events_reply","clientToken":"5S6AQZ6IDE-1","code":404,"status":"hardware_fault not defined","data":{}}
DBG|2019-09-18 15:11:59|qcloud_iot_event.c|_on_event_reply_callback(115): eventToken:5S6AQZ6IDE-1 code:404 status:hardware_fault not defined
DBG|2019-09-18 15:11:59|data_template_sample.c|event_post_cb(68): Reply:{"method":"events_reply","clientToken":"5S6AQZ6IDE-1","code":404,"status":"hardware_fault not defined","data":{}}
DBG|2019-09-18 15:11:59|qcloud_iot_event.c|_release_reply_request(78): eventToken[5S6AQZ6IDE-1] released
```

#### 执行 light_data_template_sample 示例程序
智能灯示例是基于数据数据模板的场景示例，基于此示例说明控制台和设备端控制交互及日志查询
```
./output/release/bin/light_data_template_sample
INF|2019-09-18 15:14:08|device.c|iot_device_info_set(67): SDK_Ver: 3.1.0, Product_ID: 5S6AQZ6IDE, Device_Name: dev01
DBG|2019-09-18 15:14:08|HAL_TLS_mbedtls.c|HAL_TLS_Connect(204): Setting up the SSL/TLS structure...
DBG|2019-09-18 15:14:08|HAL_TLS_mbedtls.c|HAL_TLS_Connect(246): Performing the SSL/TLS handshake...
DBG|2019-09-18 15:14:08|HAL_TLS_mbedtls.c|HAL_TLS_Connect(247): Connecting to /5S6AQZ6IDE.wx-mqtt.tencentdevices.com/8883...
INF|2019-09-18 15:14:08|HAL_TLS_mbedtls.c|HAL_TLS_Connect(269): connected with /5S6AQZ6IDE.wx-mqtt.tencentdevices.com/8883...
INF|2019-09-18 15:14:09|mqtt_client.c|IOT_MQTT_Construct(125): mqtt connect with id: JYqyP success
DBG|2019-09-18 15:14:09|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=$template/operation/result/5S6AQZ6IDE/dev01|packet_id=22146
DBG|2019-09-18 15:14:09|shadow_client.c|_shadow_event_handler(63): shadow subscribe success, packet-id=22146
INF|2019-09-18 15:14:09|light_data_template_sample.c|event_handler(235): subscribe success, packet-id=22146
INF|2019-09-18 15:14:09|shadow_client.c|IOT_Shadow_Construct(172): Sync device data successfully
INF|2019-09-18 15:14:09|light_data_template_sample.c|main(510): Cloud Device Construct Success
DBG|2019-09-18 15:14:09|mqtt_client_subscribe.c|qcloud_iot_mqtt_subscribe(138): topicName=$thing/down/event/5S6AQZ6IDE/dev01|packet_id=22147
INF|2019-09-18 15:14:09|light_data_template_sample.c|_register_data_template_property(385): data template property=power_switch registered.
INF|2019-09-18 15:14:09|light_data_template_sample.c|_register_data_template_property(385): data template property=color registered.
INF|2019-09-18 15:14:09|light_data_template_sample.c|_register_data_template_property(385): data template property=brightness registered.
INF|2019-09-18 15:14:09|light_data_template_sample.c|_register_data_template_property(385): data template property=name registered.
INF|2019-09-18 15:14:09|light_data_template_sample.c|main(531): Register data template propertys Success
DBG|2019-09-18 15:14:09|shadow_client.c|IOT_Shadow_Get(383): GET Request Document: {"clientToken":"explorer-5S6AQZ6IDE-0"}
DBG|2019-09-18 15:14:09|mqtt_client_publish.c|qcloud_iot_mqtt_publish(329): publish topic seq=22148|topicName=$template/operation/5S6AQZ6IDE/dev01|payload={"type":"get", "clientToken":"explorer-5S6AQZ6IDE-0"}
INF|2019-09-18 15:14:09|light_data_template_sample.c|event_handler(249): publish success, packet-id=22148
DBG|2019-09-18 15:14:09|shadow_client.c|_shadow_event_handler(63): shadow subscribe success, packet-id=22147
INF|2019-09-18 15:14:09|light_data_template_sample.c|event_handler(235): subscribe success, packet-id=22147
DBG|2019-09-18 15:14:09|shadow_client_manager.c|_on_operation_result_handler(278): type:get
DBG|2019-09-18 15:14:09|shadow_client.c|_update_ack_cb(114): requestAck=0
DBG|2019-09-18 15:14:09|shadow_client.c|_update_ack_cb(117): Received Json Document={"clientToken":"explorer-5S6AQZ6IDE-0","payload":{"state":{"reported":{"battery":35,"brightness":55,"color":0,"id":"19","name":"dev01","power_switch":0}},"timestamp":1568776522035,"version":401},"result":0,"timestamp":1568790849,"type":"get"}
DBG|2019-09-18 15:14:09|light_data_template_sample.c|main(602): no data need to be reported
DBG|2019-09-18 15:14:09|light_data_template_sample.c|main(609): cycle report:{"state":{"reported":{"power_switch":0,"color":0,"brightness":0,"name":"dev01"}}, "clientToken":"explorer-5S6AQZ6IDE-1"}
DBG|2019-09-18 15:14:09|shadow_client.c|IOT_Shadow_Update(317): UPDATE Request Document: {"state":{"reported":{"power_switch":0,"color":0,"brightness":0,"name":"dev01"}}, "clientToken":"explorer-5S6AQZ6IDE-1"}
DBG|2019-09-18 15:14:09|mqtt_client_publish.c|qcloud_iot_mqtt_publish(329): publish topic seq=22149|topicName=$template/operation/5S6AQZ6IDE/dev01|payload={"type":"update", "state":{"reported":{"power_switch":0,"color":0,"brightness":0,"name":"dev01"}}, "clientToken":"explorer-5S6AQZ6IDE-1"}
INF|2019-09-18 15:14:09|light_data_template_sample.c|main(613): cycle reporte success
INF|2019-09-18 15:14:10|light_data_template_sample.c|event_handler(249): publish success, packet-id=22149
DBG|2019-09-18 15:14:10|shadow_client_manager.c|_on_operation_result_handler(278): type:update
INF|2019-09-18 15:14:10|light_data_template_sample.c|OnShadowUpdateCallback(370): recv shadow update response, response ack: 0
```

## 四. 设备调试

1. 进入设备调试：
![](https://main.qcloudimg.com/raw/1f2ac1d6cac186394ac1a1da6c22749c.jpg)

2. 修改目标数据下发设备
![](https://main.qcloudimg.com/raw/911a09872f03a91d1d530537e51147f1.jpg)

3.设备响应及日志输出
![](https://main.qcloudimg.com/raw/c45d35d9abe116408efc2656b55127b7.jpg)

4. 控制台查看设备当前状态
![](https://main.qcloudimg.com/raw/236b8bf3c88b1c532714b730d0993a79.jpg)

5. 控制台查看设备通信日志
![](https://main.qcloudimg.com/raw/10e911975030f2840b9af03a079aec1d.jpg)

6. 控制台查看设备事件上报
![](https://main.qcloudimg.com/raw/d3878541b502619158ec206fc2ae2391.jpg)
