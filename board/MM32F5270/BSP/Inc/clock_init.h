/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __CLOCK_INIT_H__
#define __CLOCK_INIT_H__

#include "hal_common.h"

#define CLOCK_SYS_FREQ         120000000u
#define CLOCK_SYSTICK_FREQ     (CLOCK_SYS_FREQ/8u)
#define CLOCK_AHB1_FREQ        120000000u
#define CLOCK_APB1_FREQ        60000000u
#define CLOCK_APB2_FREQ        60000000u

void BOARD_InitBootClocks(void);

#endif /* __CLOCK_INIT_H__ */
