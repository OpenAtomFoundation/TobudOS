/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/
#ifndef _E53_SC1_H_
#define _E53_SC1_H_
 
#include "tos_k.h"
#include "stdbool.h"

#define LED_Pin GPIO_PIN_2
#define LED_GPIO_Port GPIOB

#define LED_OPEN	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET)
#define LED_CLOSE	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET)
#define LED_IS_OPEN	HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin)

#define BH1750_Addr 0x46
#define LIGHT_MAX_NAME_LEN	32

typedef enum{
    LIGHT_LEVEL_LOW = 0,
    LIGHT_LEVEL_MID = 1,
    LIGHT_LEVEL_HIGH = 2,
    LIGHT_LEVEL_BUTT,
}E_LIGTH_LEVEL;

int e53_sc1_init(void);

float e53_scl_read_data(void);

void e53_sc1_set_ligth_level(int light_level);

#endif /* _E53_SC1_H_ */
