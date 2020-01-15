/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_ILI9341_H_
#define _FSL_ILI9341_H_

#include "fsl_common.h"

/*!
 * @addtogroup ili9341
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Register defines */
#define ILI9341_CMD_RST 0x01
#define ILI9341_CMD_SLEEPOUT 0x11
#define ILI9341_CMD_GAMMA 0x26
#define ILI9341_CMD_DISPLAYOFF 0x28
#define ILI9341_CMD_DISPLAYON 0x29
#define ILI9341_CMD_COLADDR 0x2A
#define ILI9341_CMD_PAGEADDR 0x2B
#define ILI9341_CMD_GRAM 0x2C
#define ILI9341_CMD_MAC 0x36
#define ILI9341_CMD_PIXELFORMAT 0x3A
#define ILI9341_CMD_WDB 0x51
#define ILI9341_CMD_WCD 0x53
#define ILI9341_CMD_RGBINTERFACE 0xB0
#define ILI9341_CMD_FRC 0xB1
#define ILI9341_CMD_BPC 0xB5
#define ILI9341_CMD_DFC 0xB6
#define ILI9341_CMD_PWR1 0xC0
#define ILI9341_CMD_PWR2 0xC1
#define ILI9341_CMD_VCOM1 0xC5
#define ILI9341_CMD_VCOM2 0xC7
#define ILI9341_CMD_PWRA 0xCB
#define ILI9341_CMD_PWRB 0xCF
#define ILI9341_CMD_PGAMMA 0xE0
#define ILI9341_CMD_NGAMMA 0xE1
#define ILI9341_CMD_DTCA 0xE8
#define ILI9341_CMD_DTCB 0xEA
#define ILI9341_CMD_PWRSEQ 0xED
#define ILI9341_CMD_3GAMMAEN 0xF2
#define ILI9341_CMD_INTERFACE 0xF6
#define ILI9341_CMD_PRC 0xF7
#define ILI9341_CMD_INVON 0x21
#define ILI9341_CMD_INVOFF 0x20

typedef void (*ili9341_send_byte_t)(uint8_t);
void FT9341_Init(ili9341_send_byte_t _writeData, ili9341_send_byte_t _writeCommand);

#endif
