/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "flash_storage.h"
#include "ble_qiot_config.h"
#include "nrf_fstorage.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_fstorage_nvmc.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include <stdlib.h>


#ifdef SOFTDEVICE_PRESENT
#include "nrf_fstorage_sd.h"
#else
#include "nrf_fstorage_nvmc.h"
#endif

static void fstorage_evt_handler(nrf_fstorage_evt_t *p_evt);

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) = {
    /* Set a handler for fstorage events. */
    .evt_handler = fstorage_evt_handler,

    /* These below are the boundaries of the flash space assigned to this instance of fstorage.
     * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
     * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
     * last page of flash available to write data. */
    .start_addr = 0x7e000,
    .end_addr   = 0x7ffff,
};

static void *p_write_buf = NULL;

void fstorage_init(void)
{
    nrf_fstorage_api_t *p_fs_api;
    ret_code_t          rc;

#ifdef SOFTDEVICE_PRESENT
    NRF_LOG_INFO("SoftDevice is present.");
    NRF_LOG_INFO("Initializing nrf_fstorage_sd implementation...");
    /* Initialize an fstorage instance using the nrf_fstorage_sd backend.
     * nrf_fstorage_sd uses the SoftDevice to write to flash. This implementation can safely be
     * used whenever there is a SoftDevice, regardless of its status (enabled/disabled). */
    p_fs_api = &nrf_fstorage_sd;
#else
    NRF_LOG_INFO("SoftDevice not present.");
    NRF_LOG_INFO("Initializing nrf_fstorage_nvmc implementation...");
    /* Initialize an fstorage instance using the nrf_fstorage_nvmc backend.
     * nrf_fstorage_nvmc uses the NVMC peripheral. This implementation can be used when the
     * SoftDevice is disabled or not present.
     *
     * Using this implementation when the SoftDevice is enabled results in a hardfault. */
    p_fs_api = &nrf_fstorage_nvmc;
#endif

    p_write_buf = NULL;
    rc          = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);
}

/**@brief   Helper function to obtain the last address on the last page of the on-chip flash that
 *          can be used to write user data.
 *          It is possible to set the start and end addresses of an fstorage instance at runtime.
 *          They can be set multiple times, should it be needed. The helper function below can
 *          be used to determine the last address on the last page of flash memory available to
 *          store data.
 */
/*
static uint32_t nrf5_flash_end_addr_get()
{
    uint32_t const bootloader_addr = NRF_UICR->NRFFW[0];
    uint32_t const page_sz         = NRF_FICR->CODEPAGESIZE;
    uint32_t const code_sz         = NRF_FICR->CODESIZE;

    return (bootloader_addr != 0xFFFFFFFF ?
            bootloader_addr : (code_sz * page_sz));
}
*/

/**@brief   Sleep until an event is received. */
static void power_manage(void)
{
#ifdef SOFTDEVICE_PRESENT
    (void)sd_app_evt_wait();
#else
    __WFE();
#endif
}

int fstorage_read(uint32_t addr, uint32_t read_len, void *p_data)
{
    ret_code_t rc;

    /* Read data. */
    rc = nrf_fstorage_read(&fstorage, addr, p_data, read_len);
    if (rc != NRF_SUCCESS) {
        NRF_LOG_ERROR("nrf_fstorage_read() returned: %s\n", nrf_strerror_get(rc));
        return 0;
    }

    return read_len;
}

static uint32_t round_up_u32(uint32_t len)
{
    if (len % sizeof(uint32_t)) {
        return (len + sizeof(uint32_t) - (len % sizeof(uint32_t)));
    }

    return len;
}

int fstorage_erase(uint32_t addr)
{
    ret_code_t rc;

    rc = nrf_fstorage_erase(&fstorage, addr, BLE_QIOT_RECORD_FLASH_PAGENUM, NULL);
    if (rc != NRF_SUCCESS) {
        NRF_LOG_ERROR("nrf_fstorage_erase() returned: %s\n", nrf_strerror_get(rc));
        return rc;
    }

    return 0;
}

int fstorage_write(uint32_t addr, uint32_t write_len, void const *p_data)
{
    /* The following code snippet make sure that the length of the data we are writing to flash
     * is a multiple of the program unit of the flash peripheral (4 bytes).
     */
    uint32_t   len = round_up_u32(write_len);
    ret_code_t rc;

    rc = nrf_fstorage_erase(&fstorage, addr, BLE_QIOT_RECORD_FLASH_PAGENUM, NULL);
    if (rc != NRF_SUCCESS) {
        NRF_LOG_ERROR("nrf_fstorage_erase() returned: %s\n", nrf_strerror_get(rc));
    }

    /* Copy the data to write in flash.
     * Because the fstorage interface is asynchrounous, the data must be kept in memory before get
     * the NRF_FSTORAGE_EVT_WRITE_RESULT event. This memory will be free in fstorage_evt_handler().
     */
    p_write_buf = (void *)malloc(len);
    if (NULL == p_write_buf) {
        NRF_LOG_ERROR("fstorage_write() malloc size %d error", len);
        return 0;
    }

    memset(p_write_buf, 0, len);
    memcpy(p_write_buf, p_data, write_len);

    rc = nrf_fstorage_write(&fstorage, addr, p_write_buf, len, NULL);
    if (rc != NRF_SUCCESS) {
        NRF_LOG_ERROR("nrf_fstorage_write() returned: %s\n", nrf_strerror_get(rc));
        return 0;
    }

    return write_len;
}

static void fstorage_evt_handler(nrf_fstorage_evt_t *p_evt)
{
    if (p_evt->result != NRF_SUCCESS) {
        NRF_LOG_INFO("fstorage event received: ERROR while executing an fstorage operation.");
        return;
    }

    switch (p_evt->id) {
        case NRF_FSTORAGE_EVT_WRITE_RESULT: {
            free(p_write_buf);
            p_write_buf = NULL;
            NRF_LOG_INFO("fstorage event received: wrote %d bytes at address 0x%x.", p_evt->len, p_evt->addr);
        } break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT: {
            NRF_LOG_INFO("fstorage event received: erased %d page from address 0x%x.", p_evt->len, p_evt->addr);
        } break;

        default:
            break;
    }
}

/**
 * @brief While fstorage is busy, sleep and wait for an event.
 *
 * @param p_fstorage nrf_fstorage_t obj
 */
void wait_for_flash_ready(nrf_fstorage_t const *p_fstorage)
{
    /* While fstorage is busy, sleep and wait for an event. */
    while (nrf_fstorage_is_busy(p_fstorage)) {
        power_manage();
    }
}
