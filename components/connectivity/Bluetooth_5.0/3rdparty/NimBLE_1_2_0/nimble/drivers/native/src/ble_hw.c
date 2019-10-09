/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "syscfg/syscfg.h"
#include "os/os.h"
#include "nimble/ble.h"
#include "nimble/nimble_opt.h"
#include "controller/ble_hw.h"

/* Total number of white list elements supported by nrf52 */
#define BLE_HW_WHITE_LIST_SIZE      (0)

/* We use this to keep track of which entries are set to valid addresses */
static uint8_t g_ble_hw_whitelist_mask;

/* Returns public device address or -1 if not present */
int
ble_hw_get_public_addr(ble_addr_t *addr)
{
    return -1;
}

/* Returns random static address or -1 if not present */
int
ble_hw_get_static_addr(ble_addr_t *addr)
{
    return -1;
}

/**
 * Clear the whitelist
 *
 * @return int
 */
void
ble_hw_whitelist_clear(void)
{
    g_ble_hw_whitelist_mask = 0;
}

/**
 * Add a device to the hw whitelist
 *
 * @param addr
 * @param addr_type
 *
 * @return int 0: success, BLE error code otherwise
 */
int
ble_hw_whitelist_add(uint8_t *addr, uint8_t addr_type)
{
    return BLE_ERR_MEM_CAPACITY;
}

/**
 * Remove a device from the hw whitelist
 *
 * @param addr
 * @param addr_type
 *
 */
void
ble_hw_whitelist_rmv(uint8_t *addr, uint8_t addr_type)
{
    return;
}

/**
 * Returns the size of the whitelist in HW
 *
 * @return int Number of devices allowed in whitelist
 */
uint8_t
ble_hw_whitelist_size(void)
{
    return BLE_HW_WHITE_LIST_SIZE;
}

/**
 * Enable the whitelisted devices
 */
void
ble_hw_whitelist_enable(void)
{
    return;
}

/**
 * Disables the whitelisted devices
 */
void
ble_hw_whitelist_disable(void)
{
    return;
}

/**
 * Boolean function which returns true ('1') if there is a match on the
 * whitelist.
 *
 * @return int
 */
int
ble_hw_whitelist_match(void)
{
    return 0;
}

/* Encrypt data */
int
ble_hw_encrypt_block(struct ble_encryption_block *ecb)
{
    return -1;
}

/**
 * Initialize the random number generator
 *
 * @param cb
 * @param bias
 *
 * @return int
 */
int
ble_hw_rng_init(ble_rng_isr_cb_t cb, int bias)
{
    return -1;
}

/**
 * Start the random number generator
 *
 * @return int
 */
int
ble_hw_rng_start(void)
{
    return -1;
}

/**
 * Stop the random generator
 *
 * @return int
 */
int
ble_hw_rng_stop(void)
{
    return -1;
}

/**
 * Read the random number generator.
 *
 * @return uint8_t
 */
uint8_t
ble_hw_rng_read(void)
{
    return 0;
}

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PRIVACY)
/**
 * Clear the resolving list
 *
 * @return int
 */
void
ble_hw_resolv_list_clear(void)
{
}

/**
 * Add a device to the hw resolving list
 *
 * @param irk   Pointer to IRK to add
 *
 * @return int 0: success, BLE error code otherwise
 */
int
ble_hw_resolv_list_add(uint8_t *irk)
{
    return BLE_ERR_MEM_CAPACITY;
}

/**
 * Remove a device from the hw resolving list
 *
 * @param index Index of IRK to remove
 */
void
ble_hw_resolv_list_rmv(int index)
{
}

/**
 * Returns the size of the resolving list. NOTE: this returns the maximum
 * allowable entries in the HW. Configuration options may limit this.
 *
 * @return int Number of devices allowed in resolving list
 */
uint8_t
ble_hw_resolv_list_size(void)
{
    return 0;
}

/**
 * Called to determine if the address received was resolved.
 *
 * @return int  Negative values indicate unresolved address; positive values
 *              indicate index in resolving list of resolved address.
 */
int
ble_hw_resolv_list_match(void)
{
    return -1;
}
#endif
