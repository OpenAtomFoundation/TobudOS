#! /bin/bash

sed -i "s/QCLOUD_ERR_SUCCESS/QCLOUD_RET_SUCCESS/g" `grep -rwl QCLOUD_ERR_SUCCESS ./*`
sed -i "s/QCLOUD_ERR_MQTT_RECONNECTED/QCLOUD_RET_MQTT_RECONNECTED/g" `grep -rwl QCLOUD_ERR_MQTT_RECONNECTED ./*`
sed -i "s/QCLOUD_ERR_MQTT_MANUALLY_DISCONNECTED/QCLOUD_RET_MQTT_MANUALLY_DISCONNECTED/g" `grep -rwl QCLOUD_ERR_MQTT_MANUALLY_DISCONNECTED ./*`
sed -i "s/QCLOUD_ERR_MQTT_CONNACK_CONNECTION_ACCEPTED/QCLOUD_RET_MQTT_CONNACK_CONNECTION_ACCEPTED/g" `grep -rwl QCLOUD_ERR_MQTT_CONNACK_CONNECTION_ACCEPTED ./*`
sed -i "s/QCLOUD_ERR_MQTT_ALREADY_CONNECTED/QCLOUD_RET_MQTT_ALREADY_CONNECTED/g" `grep -rwl QCLOUD_ERR_MQTT_ALREADY_CONNECTED ./*` 
sed -i "s/MAX_SIZE_OF_DEVICE_SERC/MAX_SIZE_OF_DEVICE_SECRET/g" `grep -rwl MAX_SIZE_OF_DEVICE_SERC ./*`
sed -i "s/devCertFileName/dev_cert_file_name/g" `grep -rwl devCertFileName ./*`
sed -i "s/devPrivateKeyFileName/dev_key_file_name/g" `grep -rwl devPrivateKeyFileName ./*`
sed -i "s/devSerc/device_secret/g" `grep -rwl devSerc ./*`
sed -i "s/MAX_SIZE_OF_PRODUCT_KEY/MAX_SIZE_OF_PRODUCT_SECRET/g" `grep -rwl MAX_SIZE_OF_PRODUCT_KEY ./*`
sed -i "s/product_key/product_secret/g" `grep -rwl product_key ./*` 
sed -i "s/DEBUG/eLOG_DEBUG/g" `grep -rwl DEBUG ./*`
sed -i "s/INFO/eLOG_INFO/g" `grep -rwl INFO ./*`
sed -i "s/WARN/eLOG_WARN/g" `grep -rwl WARN ./*`
sed -i "s/ERROR/eLOG_ERROR/g" `grep -rwl ERROR ./*`
sed -i "s/DISABLE/eLOG_DISABLE/g" `grep -rwl DISABLE ./*`
sed -i "s/Log_writter/IOT_Log_Gen/g" `grep -rwl Log_writter ./*` 
sed -i "s/qcloud_iot_dyn_reg_dev/IOT_DynReg_Device/g" `grep -rwl qcloud_iot_dyn_reg_dev ./*`
sed -i "s/IOT_SYSTEM_GET_TIME/IOT_Get_SysTime/g" `grep -rwl IOT_SYSTEM_GET_TIME ./*`

