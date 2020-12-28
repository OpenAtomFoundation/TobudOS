/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __APOLLO3_SERVICE_H
#define __APOLLO3_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "ble_qiot_export.h"


#define QIOT_START_HDL 0x0501 // do not start from 1, gap/gatt characteristic use some hdl from 1
#define QIOT_END_HDL   (QIOT_MAX_HDL - 1)

enum
{
    QIOT_SVC_HDL = QIOT_START_HDL,
    QIOT_DEVICE_INFO_CHAR_HDL,
    QIOT_DEVICE_INFO_HDL,
    QIOT_DATA_CHAR_HDL,
    QIOT_DATA_HDL,
    QIOT_EVENT_CHAR_HDL,
    QIOT_EVENT_HDL,
    QIOT_EVENT_CCC_HDL,
    QIOT_MAX_HDL,
};

void ble_services_add(const qiot_service_init_s *p_service);

#ifdef __cplusplus
}
#endif

#endif // __APOLLO3_SERVICE_H
