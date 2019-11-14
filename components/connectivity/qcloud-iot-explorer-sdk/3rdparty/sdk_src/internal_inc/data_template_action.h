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

#ifndef _DATA_TEMPLATE_ACTION_H_
#define _DATA_TEMPLATE_ACTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>


#define ACTION_NAME_MAX_LEN			(20)  // action name max len
#define ACTION_TOKEN_MAX_LEN	    (32)  // action token max len

#define MAX_ACTION_WAIT_REPLY       (10)  
#define ACTION_MAX_DATA_NUM		    (255) // input or output max data num

#define REPORT_ACTION			"action_reply"
#define CALL_ACTION			    "action"

int IOT_Action_Init(void *c);

int IOT_Action_Register(void *pTemplate, DeviceAction *pAction, OnActionHandleCallback callback);

int IOT_Action_Remove(void *ptemplate, DeviceAction *pAction);

#ifdef __cplusplus
}
#endif

#endif //_DATA_TEMPLATE_ACTION_H_
