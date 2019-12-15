/**
 * @file    wm_include.h
 *
 * @brief   the configuration file of sdk
 *
 * @author  winnermicro
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef __WM_INCLUDE_H__
#define __WM_INCLUDE_H__

/**
 * @mainpage WinnerMicro SDK
 *
 * Quick Start of WinnerMicro SDK.
 *
 *
 * HOW TO CODE ?
 *
 * Function UserMain(void) is the entrance function of the application:
 * @code
 * void UserMain(void)
 * {
 *     printf("\n user task\n");
 *
 * #if DEMO_CONSOLE
 *	   CreateDemoTask();
 * #endif
 *
 *     //user's task
 * }
 * @endcode
 *
 *
 * \n
 * HOW TO COMPILE ?
 *
 * To build with the SDK you can use the keil tools.
 * Opening "Tools\Keil\Project\WM_SDK.uvproj" to compile.
 *
 *
 * \n
 * HOW TO DOWNLOAD THE FIRMWARE ?
 *
 * Download the "WM_W600.FLS" image
 *
 * This will download image which includes secboot & sdk into flash by ROM using xModem-protocol.
 * @code
 * Pulling down the bootmode pin and reset the device. Then UART0 will output: 
 * CCC...
 * For details,please refer to the sdk manual.
 * @endcode
 *
 * Download the "WM_W600_SEC.IMG" image
 *
 * This will download image which includes sdk by secboot using xmodem-protocol.
 * @code
 * Press "ESC" and then reset the device. Then UART0 will output:
 * secboot running...
 * CCC...
 * For details,please refer to the sdk manual.
 * @endcode
 *
 * \n
 */

#include <stdio.h>
#include <stdlib.h>
#include "wm_type_def.h"
#include "wm_gpio.h"
//#include "wm_flash.h"
#include "wm_regs.h"

#endif
