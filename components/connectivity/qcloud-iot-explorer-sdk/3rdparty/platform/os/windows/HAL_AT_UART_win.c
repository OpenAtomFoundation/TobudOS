/*
 * Copyright (c) 2019-2021 Tencent Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "qcloud_iot_import.h"
#include "qcloud_iot_export.h"

#ifdef AT_TCP_ENABLED

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


#include "utils_ringbuff.h"
#include "at_client.h"


/**
* @brief This function handles AT UART global interrupt,push recv char to ringbuff.
*/
void HAL_AT_UART_IRQHandler(void)
{ 

}


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
