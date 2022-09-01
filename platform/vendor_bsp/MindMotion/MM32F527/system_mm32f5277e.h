/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef __SYSTEM_MM32F5277E_H__
#define __SYSTEM_MM32F5277E_H__

/* define the CP10 & CP11 full access to the FP Extension. */
#define SCB_CPACR_CP10_MASK ( 0x03 << 20u)
#define SCB_CPACR_CP11_MASK (0x03 << 22u)

void SystemInit(void);

#endif /* __SYSTEM_MM32F5277E_H__ */
