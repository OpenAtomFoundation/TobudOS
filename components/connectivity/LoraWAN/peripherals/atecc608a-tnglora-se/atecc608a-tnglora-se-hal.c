/**
 * @file      atecc608a-tnglora-se-hal.c
 *
 * @brief     Secure Element hardware abstraction layer implementation
 *
 * @remark    Current implementation only supports LoRaWAN 1.0.x version
 * 
 * @copyright Copyright (c) 2020 The Things Industries B.V.
 * 
 * Revised BSD License
 * Copyright The Things Industries B.V 2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Things Industries B.V nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE THINGS INDUSTRIES B.V BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <stdio.h>

#include "atca_hal.h"
#include "atca_device.h"
#include "atca_execution.h"
#include "atca_status.h"
#include "i2c-board.h" 
#include "delay.h"

#include "radio.h"

#include "atecc608a-tnglora-se-hal.h"

uint32_t ATECC608ASeHalGetRandomNumber( void )
{
    return Radio.Random( );
}

/** @brief This function delays for a number of microseconds.
 *
 * @param[in] delay number of 0.001 milliseconds to delay
 */
void atca_delay_us(uint32_t delay)
{
    // use ASF supplied delay
    DelayMs(delay / 1000);
}

/** @brief This function delays for a number of tens of microseconds.
 *
 * @param[in] delay number of 0.01 milliseconds to delay
 */
void atca_delay_10us(uint32_t delay)
{
    // use ASF supplied delay
    DelayMs(delay / 100);
}

/** @brief This function delays for a number of milliseconds.
 *
 *         You can override this function if you like to do
 *         something else in your system while delaying.
 * @param[in] delay number of milliseconds to delay
 */
void atca_delay_ms(uint32_t delay)
{
    // use ASF supplied delay
    DelayMs(delay);
}

/** @brief discover i2c buses available for this hardware 
 * this maintains a list of logical to physical bus mappings freeing the application 
 * of the a-priori knowledge 
 * @param[in] i2c_buses - an array of logical bus numbers 
 * @param[in] max_buses - maximum number of buses the app wants to attempt to discover 
 * @return ATCA_SUCCESS 
 */

ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
    return ATCA_SUCCESS;
}

/** @brief discover any CryptoAuth devices on a given logical bus number 
 * @param[in]  bus_num  logical bus number on which to look for CryptoAuth devices 
 * @param[out] cfg     pointer to head of an array of interface config structures which get filled in by this method 
 * @param[out] found   number of devices found on this bus 
 * @return ATCA_SUCCESS 
 */

ATCA_STATUS hal_i2c_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    return ATCA_SUCCESS;
}

/** @brief 
    - this HAL implementation assumes you've included the ASF SERCOM I2C libraries in your project, otherwise, 
    the HAL layer will not compile because the ASF I2C drivers are a dependency * 
 */

/** @brief hal_i2c_init manages requests to initialize a physical interface.  it manages use counts so when an interface 
 * has released the physical layer, it will disable the interface for some other use. 
 * You can have multiple ATCAIFace instances using the same bus, and you can have multiple ATCAIFace instances on 
 * multiple i2c buses, so hal_i2c_init manages these things and ATCAIFace is abstracted from the physical details. 
 */

/** @brief initialize an I2C interface using given config 
 * @param[in] hal - opaque ptr to HAL data 
 * @param[in] cfg - interface configuration 
 * @return ATCA_SUCCESS on success, otherwise an error code. 
 */
ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
    return ATCA_SUCCESS;
}

/** @brief HAL implementation of I2C post init 
 * @param[in] iface  instance 
 * @return ATCA_SUCCESS 
 */
ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/** @brief HAL implementation of I2C send over ASF 
 * @param[in] iface     instance 
 * @param[in] txdata    pointer to space to bytes to send 
 * @param[in] txlength  number of bytes to send 
 * @return ATCA_SUCCESS on success, otherwise an error code. 
 */

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t *txdata, int txlength)
{

    txdata[0] = 0x3;
    txlength++;
    if (I2cMcuWriteBuffer((I2c_t *)NULL, iface->mIfaceCFG->atcai2c.slave_address, 0, txdata, (size_t)txlength) == 1)
    {
        return ATCA_SUCCESS;
    }
    else
    {
        return ATCA_TX_FAIL;
    }
}

/** @brief HAL implementation of I2C receive function for ASF I2C 
 * @param[in]    iface     Device to interact with. 
 * @param[out]   rxdata    Data received will be returned here. 
 * @param[inout] rxlength  As input, the size of the rxdata buffer. 
 *                         As output, the number of bytes received. 
 * @return ATCA_SUCCESS on success, otherwise an error code. 
 */
ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength)
{
    // read procedure is:
    // 1. read 1 byte, this will be the length of the package
    // 2. read the rest of the package

    uint8_t lengthPackage[1] = {0};
    int r = -1;
    int retries = iface->mIfaceCFG->rx_retries;
    while (--retries > 0 && r != 1)
    {
        r = I2cMcuReadBuffer((I2c_t *)NULL, iface->mIfaceCFG->atcai2c.slave_address, 0, lengthPackage, 1);
    }

    if (r != 1)
    {
        return ATCA_RX_TIMEOUT;
    }

    uint8_t bytesToRead = lengthPackage[0] - 1;

    if (bytesToRead > *rxlength)
    {
        printf("hal_i2c_receive buffer too small, requested %u, but have %u", bytesToRead, *rxlength);
        return ATCA_SMALL_BUFFER;
    }

    memset(rxdata, 0, *rxlength);
    rxdata[0] = lengthPackage[0];

    r = -1;
    retries = iface->mIfaceCFG->rx_retries;
    while (--retries > 0 && r != 1)
    {
        r = I2cMcuReadBuffer((I2c_t *)NULL, iface->mIfaceCFG->atcai2c.slave_address, 0, rxdata + 1, bytesToRead);
    }

    if (r != 1)
    {
        return ATCA_RX_TIMEOUT;
    }

    *rxlength = lengthPackage[0];

    return ATCA_SUCCESS;
}

/** @brief method to change the bus speec of I2C 
 * @param[in] iface  interface on which to change bus speed 
 * @param[in] speed  baud rate (typically 100000 or 400000) 
 */

void change_i2c_speed(ATCAIface iface, uint32_t speed)
{
    return;
}

/** @brief wake up CryptoAuth device using I2C bus 
 * @param[in] iface  interface to logical device to wakeup 
 * @return ATCA_SUCCESS on success, otherwise an error code. 
 */

ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
    // 2. Send NULL buffer to address 0x0 (NACK)
    uint8_t emptybuff[1] = {0};
    int r = I2cMcuWriteBuffer((I2c_t *)NULL, 0x00, 0, emptybuff, (size_t)0);

    // 3. Wait for wake_delay
    atca_delay_us(iface->mIfaceCFG->wake_delay);

    uint8_t rx_buffer[4] = {0};

    // 4. Read from normal slave_address
    r = -1;
    int retries = iface->mIfaceCFG->rx_retries;
    while (--retries > 0 && r != 1)
    {
        r = I2cMcuReadBuffer((I2c_t *)NULL, iface->mIfaceCFG->atcai2c.slave_address, 0, rx_buffer, 4);
    }

    // 5. Set frequency back to requested one
    const uint8_t expected_response[4] = {0x04, 0x11, 0x33, 0x43};
    uint8_t selftest_fail_resp[4] = {0x04, 0x07, 0xC4, 0x40};

    if (memcmp(rx_buffer, expected_response, 4) == 0)
    {
        return ATCA_SUCCESS;
    }
    if (memcmp(rx_buffer, selftest_fail_resp, 4) == 0)
    {
        return ATCA_STATUS_SELFTEST_ERROR;
    }
    return ATCA_WAKE_FAILED;
}

/** @brief idle CryptoAuth device using I2C bus 
 * @param[in] iface  interface to logical device to idle 
 * @return ATCA_SUCCESS on success, otherwise an error code. 
 */

ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
    uint8_t buffer[1] = { 0x2 }; // idle word address value 
    I2cMcuWriteBuffer((I2c_t*)NULL, iface->mIfaceCFG->atcai2c.slave_address,0, buffer, (size_t)1); 
    return ATCA_SUCCESS;
}

/** @brief sleep CryptoAuth device using I2C bus 
 * @param[in] iface  interface to logical device to sleep 
 * @return ATCA_SUCCESS on success, otherwise an error code. 
 */

ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
    uint8_t buffer[1] = { 0x1 };  // sleep word address value 
    I2cMcuWriteBuffer((I2c_t*)NULL, iface->mIfaceCFG->atcai2c.slave_address,0, buffer, (size_t)1); 
    return ATCA_SUCCESS;
}

/** @brief manages reference count on given bus and releases resource if no more refences exist 
 * @param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation 
 * return ATCA_SUCCESS 
 */

ATCA_STATUS hal_i2c_release(void *hal_data)
{
    return ATCA_SUCCESS;
}
