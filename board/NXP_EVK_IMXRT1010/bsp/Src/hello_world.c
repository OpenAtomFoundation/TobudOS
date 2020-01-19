/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "cmsis_os.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
#define TASK1_STK_SIZE          512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE          512
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);


void task1(void *arg)
{
	int count = 0;
	while(1)
	{
		PRINTF("********This is Task 1, count is %d \r\n",count++);
		osDelay(1000);
	}
}


void task2(void *arg)
{
	int count = 0;
	while(1)
	{
		PRINTF("++++++++This is Task 2, count is %d \r\n",count++);
		osDelay(2000);
	}
}


/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    PRINTF("Hello world from i.MX RT1010.\r\n");
    PRINTF("Welcome to TencentOS tiny\r\n");
    osKernelInitialize(); // TencentOS Tiny kernel initialize
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
    osKernelStart(); // Start TencentOS Tiny
}
