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

#ifndef IOT_DATA_TEMPLATE_CLIENT_COMMON_H_
#define IOT_DATA_TEMPLATE_CLIENT_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "data_template_client.h"

/**
* @brief register a device property
*
* @param pTemplate handle to data_template client
* @param pProperty device property
* @param callback  callback when property changes
* @return          QCLOUD_RET_SUCCESS for success, or err code for failure
*/

int template_common_register_property_on_delta(Qcloud_IoT_Template *pTemplate, DeviceProperty *pProperty, OnPropRegCallback callback);

/**
 * @brief remove a device property
 *
 * @param pTemplate handle to data_template client
 * @param pProperty device property
 * @return          QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int template_common_remove_property(Qcloud_IoT_Template *ptemplate, DeviceProperty *pProperty);

/**
 * @brief check if a device property exists
 *
 * @param pShadow   handle to data_template client
 * @param pProperty device property
 * @return          0 = not existed
 */ 
int template_common_check_property_existence(Qcloud_IoT_Template *ptemplate, DeviceProperty *pProperty);


#ifdef __cplusplus
}
#endif

#endif //IOT_DATA_TEMPLATE_CLIENT_COMMON_H_
