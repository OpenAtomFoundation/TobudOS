/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#ifdef AT_TCP_ENABLED

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "at_client.h"
#include "utils_ringbuff.h"

/**
 * @brief This function handles AT UART global interrupt,push recv char to ringbuff.
 */
void HAL_AT_UART_IRQHandler(void) {}

/**
 *pdata: pointer of data for send
 *len:  len of data to be sent
 *return: the len of data send success
 * @brief hal api for at data send
 */
int HAL_AT_Uart_Send(void *data, uint32_t size)
{
    return 0;
}

int HAL_AT_Uart_Init(void)
{
    return QCLOUD_RET_SUCCESS;
}

int HAL_AT_Uart_Deinit(void)
{
    return QCLOUD_RET_SUCCESS;
}

#endif
