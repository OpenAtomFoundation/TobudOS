/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef QCLOUD_IOT_EXPORT_H_
#define QCLOUD_IOT_EXPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "platform.h"


/* IoT C-SDK version info */
#define QCLOUD_IOT_DEVICE_SDK_VERSION                               "3.1.2"

/**************** QCloud IoT C-SDK constants begin ************************/

/* MAX size of client ID */
#define MAX_SIZE_OF_CLIENT_ID                                       (80)

/* MAX size of product ID */
#define MAX_SIZE_OF_PRODUCT_ID                                    	(10)

/* MAX size of product secret */
#define MAX_SIZE_OF_PRODUCT_SECRET                                    	(32)

/* MAX size of device name */
#define MAX_SIZE_OF_DEVICE_NAME                                     (48)

/* MAX size of device secret */
#define MAX_SIZE_OF_DEVICE_SECRET                                     (64)

/* MAX size of device cert file name */
#define MAX_SIZE_OF_DEVICE_CERT_FILE_NAME                           (128)

/* MAX size of device key file name */
#define MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME                            (128)

/**************** QCloud IoT C-SDK constants end *************************/

typedef struct {
	char	product_id[MAX_SIZE_OF_PRODUCT_ID + 1];
	char 	device_name[MAX_SIZE_OF_DEVICE_NAME + 1];
	char	client_id[MAX_SIZE_OF_CLIENT_ID + 1];
	
#ifdef AUTH_MODE_CERT
	char  	dev_cert_file_name[MAX_SIZE_OF_DEVICE_CERT_FILE_NAME + 1];
	char 	dev_key_file_name[MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME + 1];
#else
	char	device_secret[MAX_SIZE_OF_DEVICE_SECRET + 1];
#endif

#ifdef DEV_DYN_REG_ENABLED
	char	product_secret[MAX_SIZE_OF_PRODUCT_SECRET + 1];
#endif  	
} DeviceInfo;

#ifdef GATEWAY_ENABLED
typedef struct {
	DeviceInfo gw_info;
	DeviceInfo *sub_dev_info;
	unsigned int sub_dev_num;
} GatewayDeviceInfo;
#endif 

#include "qcloud_iot_export_variables.h"
#include "qcloud_iot_export_coap.h"
#include "qcloud_iot_export_log.h"
#include "qcloud_iot_export_error.h"
#include "qcloud_iot_export_mqtt.h"
#include "qcloud_iot_export_shadow.h"
#include "qcloud_iot_export_ota.h"
#include "qcloud_iot_export_system.h"
#include "qcloud_iot_export_gateway.h"
#include "qcloud_iot_export_dynreg.h"


#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_H_ */
