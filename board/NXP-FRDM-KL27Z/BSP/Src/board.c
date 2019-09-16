/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "board.h"
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_i2c.h"
#endif /* SDK_I2C_BASED_COMPONENT_USED */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
    uint32_t uartClkSrcFreq;
    /* SIM_SOPT2[27:26]:
     *  00: Clock Disabled
     *  01: IRC48M
     *  10: OSCERCLK
     *  11: MCGIRCCLK
     */
    CLOCK_SetLpuart0Clock(1);

    uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ;
    DbgConsole_Init(BOARD_DEBUG_UART_BASEADDR, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
}

void board_init(void)
{
    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    PRINTF("Welcome to TencentOS tiny.\r\n");

}

#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
void BOARD_I2C_Init(I2C_Type *base, uint32_t clkSrc_Hz)
{
    i2c_master_config_t i2cConfig = {0};

    I2C_MasterGetDefaultConfig(&i2cConfig);
    I2C_MasterInit(base, &i2cConfig, clkSrc_Hz);
}

status_t BOARD_I2C_Send(I2C_Type *base,
                        uint8_t deviceAddress,
                        uint32_t subAddress,
                        uint8_t subaddressSize,
                        uint8_t *txBuff,
                        uint8_t txBuffSize)
{
    i2c_master_transfer_t masterXfer;

    /* Prepare transfer structure. */
    masterXfer.slaveAddress = deviceAddress;
    masterXfer.direction = kI2C_Write;
    masterXfer.subaddress = subAddress;
    masterXfer.subaddressSize = subaddressSize;
    masterXfer.data = txBuff;
    masterXfer.dataSize = txBuffSize;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    return I2C_MasterTransferBlocking(base, &masterXfer);
}

status_t BOARD_I2C_Receive(I2C_Type *base,
                           uint8_t deviceAddress,
                           uint32_t subAddress,
                           uint8_t subaddressSize,
                           uint8_t *rxBuff,
                           uint8_t rxBuffSize)
{
    i2c_master_transfer_t masterXfer;

    /* Prepare transfer structure. */
    masterXfer.slaveAddress = deviceAddress;
    masterXfer.subaddress = subAddress;
    masterXfer.subaddressSize = subaddressSize;
    masterXfer.data = rxBuff;
    masterXfer.dataSize = rxBuffSize;
    masterXfer.direction = kI2C_Read;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    return I2C_MasterTransferBlocking(base, &masterXfer);
}

void BOARD_Accel_I2C_Init(void)
{
    BOARD_I2C_Init(BOARD_ACCEL_I2C_BASEADDR, BOARD_ACCEL_I2C_CLOCK_FREQ);
}

status_t BOARD_Accel_I2C_Send(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint32_t txBuff)
{
    uint8_t data = (uint8_t)txBuff;

    return BOARD_I2C_Send(BOARD_ACCEL_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, &data, 1);
}

status_t BOARD_Accel_I2C_Receive(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_I2C_Receive(BOARD_ACCEL_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, rxBuff, rxBuffSize);
}
#endif /* SDK_I2C_BASED_COMPONENT_USED */
