/** ****************************************************************************
 * @copyright                  Nationz Co.Ltd
 *               Copyright (c) 2013гн2018 All Rights Reserved
 *******************************************************************************
 * @file     am_hal_global.c
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Locate global variables here.
 *
 * This module contains global variables that are used throughout the HAL.
 *
 * One use in particular is that it uses a global HAL flags variable that
 * contains flags used in various parts of the HAL.
******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_Z32HLP.h"

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
uint32_t volatile g_ui32HALflags = 0x00000000;
