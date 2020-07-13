## 简介
腾讯云物联网提供了专用的 [腾讯云IoT AT指令集](https://github.com/tencentyun/qcloud-iot-sdk-tencent-at-based/blob/master/docs/%E8%85%BE%E8%AE%AF%E4%BA%91IoT%20AT%E6%8C%87%E4%BB%A4%E9%9B%86-V3.1.3.pdf)，如果通讯模组实现了该指令集，则设备接入和通讯更为简单，所需代码量更少，针对这种场景，请参考面向腾讯云定制AT模组专用的 [MCU AT SDK](https://github.com/tencentyun/qcloud-iot-sdk-tencent-at-based.git)

目前腾讯云和主流的模组厂商进行了深度合作，将SDK的核心协议已移植到模组中，模组对外封装统一的腾讯云AT指令。已支持腾讯云定制AT指令的模组列表如下：

| 序号  | 模组商     |   模组型号       |  通信制式      |   固件版本      |
| -------| ------------| -------------------|------------------|----------------|
| 1      | 中移        |   M5311          |      NB-IoT    |  M5311_LV_MOD_BOM_R002_1901232019_0906   |
| 2      | 中移        |   M6315         |          2G    |   CMIOT_M6315_20180901_V10_EXT_20190827_152209     |
| 3     | 中移         |   M8321          |         4G     |   QCloud_AT_v3.0.1_4G_Cellular 20190909_171245     |
| 4      | 有方               | N10              |     2G         |     N10_I_1187_PQS63010_TC_V002C   |
| 5      | 有方               | N21                |    NB-IoT      |     N21_RDD0CM_TC_V006A   |
| 6      | 有方               | N720            |    4G         |    N720_EAB0CMF_BZ_V003A_T1    |
| 7      | 移柯               | L206            |    2G         | L206Dv01.04b04.04 |
| 8      | 乐鑫               | ESP8266            |    WIFI         |   QCloud_AT_ESP_WiFi_v1.1.0     |

