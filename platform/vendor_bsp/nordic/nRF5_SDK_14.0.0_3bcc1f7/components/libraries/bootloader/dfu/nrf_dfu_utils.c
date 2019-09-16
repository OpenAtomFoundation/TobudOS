/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "nrf_dfu_utils.h"

#include <string.h>
#include "nrf_dfu_settings.h"
#include "nrf_dfu_mbr.h"
#include "nrf_bootloader_app_start.h"
#include "nrf_bootloader_info.h"
#include "crc32.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "app_timer.h"
#include "nrf_delay.h"

static app_timer_t nrf_dfu_post_sd_bl_timeout_timer = { {0} };
const app_timer_id_t nrf_dfu_post_sd_bl_timeout_timer_id = &nrf_dfu_post_sd_bl_timeout_timer;

/**
 * Round up val to the next page boundry
 */
static uint32_t align_to_page(uint32_t val)
{
    return ((val + CODE_PAGE_SIZE - 1 ) &~ (CODE_PAGE_SIZE - 1));
}


static void nrf_dfu_invalidate_bank(nrf_dfu_bank_t * p_bank)
{
    // Set the bank-code to invalid, and reset size/CRC
    memset(p_bank, 0, sizeof(nrf_dfu_bank_t));

    // Reset write pointer after completed operation
    s_dfu_settings.write_offset = 0;

    // Reset SD size
    s_dfu_settings.sd_size = 0;

    // Promote dual bank layout
    s_dfu_settings.bank_layout = NRF_DFU_BANK_LAYOUT_DUAL;

    // Signify that bank 0 is empty
    s_dfu_settings.bank_current = NRF_DFU_CURRENT_BANK_0;
}


/** @brief Function to continue application update.
 *
 * @details This function will be called after reset if there is a valid application in Bank1
 *          required to be copied down to Bank 0.
 *
 * @param[in]       src_addr            Source address of the application to copy from Bank1 to Bank0.
 *
 * @retval  NRF_SUCCESS                 Continuation was successful.
 * @retval  NRF_ERROR_NULL              Invalid data during compare.
 * @retval  FS_ERR_UNALIGNED_ADDR       A call to fstorage was not aligned to a page boundary or the address was not word aliged.
 * @retval  FS_ERR_INVALID_ADDR         The destination of a call to fstorage does not point to
 *                                      the start of a flash page or the operation would
 *                                      go beyond the flash memory boundary.
 * @retval  FS_ERR_NOT_INITIALIZED      The fstorage module is not initialized.
 * @retval  FS_ERR_INVALID_CFG          The initialization of the fstorage module is invalid.
 * @retval  FS_ERR_NULL_ARG             A call to fstorage had an invalid NULL argument.
 * @retval  FS_ERR_INVALID_ARG          A call to fstorage had invalid arguments.
 * @retval  FS_ERR_QUEUE_FULL           If the internal operation queue of the fstorage module is full.
 * @retval  FS_ERR_FAILURE_SINCE_LAST   If an error occurred in another transaction and fstorage cannot continue before
 *                                      the event has been dealt with.
 */
static uint32_t nrf_dfu_app_continue(uint32_t src_addr)
{
    // This function is only in use when new app is present in Bank 1
    uint32_t const image_size  = s_dfu_settings.bank_1.image_size;
    uint32_t const split_size  = CODE_PAGE_SIZE; // Must be page aligned

    uint32_t ret_val     = NRF_SUCCESS;
    uint32_t target_addr = MAIN_APPLICATION_START_ADDR + s_dfu_settings.write_offset;
    uint32_t length_left = (image_size - s_dfu_settings.write_offset);
    uint32_t cur_len;
    uint32_t crc;

    NRF_LOG_DEBUG("Enter nrf_dfu_app_continue");

    src_addr += s_dfu_settings.write_offset;

    // Copy the application down safely
    do
    {
        cur_len = (length_left > split_size) ? split_size : length_left;

        // Erase the target page
        ret_val = nrf_dfu_flash_erase(target_addr, split_size / CODE_PAGE_SIZE, NULL);
        if (ret_val != NRF_SUCCESS)
        {
            return ret_val;
        }

        // Flash one page
        ret_val = nrf_dfu_flash_store(target_addr, (uint32_t*)src_addr, cur_len, NULL);
        if (ret_val != NRF_SUCCESS)
        {
            return ret_val;
        }

        ret_val = nrf_dfu_mbr_compare((uint32_t*)target_addr, (uint32_t*)src_addr, cur_len);
        if (ret_val != NRF_SUCCESS)
        {
            // We will not retry the copy
            NRF_LOG_ERROR("Invalid data during compare: target: 0x%08x, src: 0x%08x", target_addr, src_addr);
            return ret_val;
        }

        s_dfu_settings.write_offset += cur_len;
        ret_val = nrf_dfu_settings_write(NULL);

        target_addr += cur_len;
        src_addr += cur_len;

        length_left -= cur_len;
    }
    while (length_left > 0);

    // Check the CRC of the copied data. Enable if so.
    crc = crc32_compute((uint8_t*)MAIN_APPLICATION_START_ADDR, image_size, NULL);

    if (crc == s_dfu_settings.bank_1.image_crc)
    {
        NRF_LOG_DEBUG("Setting app as valid");
        s_dfu_settings.bank_0.bank_code = NRF_DFU_BANK_VALID_APP;
        s_dfu_settings.bank_0.image_crc = crc;
        s_dfu_settings.bank_0.image_size = image_size;
    }
    else
    {
        NRF_LOG_ERROR("CRC computation failed for copied app: "
                      "src crc: 0x%08x, res crc: 0x%08x",
                      s_dfu_settings.bank_1.image_crc,
                      crc);
    }

    nrf_dfu_invalidate_bank(&s_dfu_settings.bank_1);
    ret_val = nrf_dfu_settings_write(NULL);

    return ret_val;
}

/**
 * @brief Flash storage callback used to reset the device if no new DFU is initiated within the timer's expiration.
 *
 * After the completion of a SD, BL or SD + BL update, the controller might want to update the
 * application as well. Because of this, the DFU target will stay in bootloader mode for some
 * time after completion. However, if no such update is received, the device should reset
 * to look for a valid app and resume regular operation.
 */
static void reset_device_callback(nrf_fstorage_evt_t * p_evt)
{
    uint32_t err_code;

    // Verify that the current application is valid.
    if (nrf_dfu_app_is_valid())
    {
        NRF_LOG_DEBUG("Starting reset delay timer");

        err_code = app_timer_create(&nrf_dfu_post_sd_bl_timeout_timer_id, APP_TIMER_MODE_SINGLE_SHOT, nrf_dfu_reset_timeout_handler);
        APP_ERROR_CHECK(err_code);
        // 3400 ms is the smallest stable value with nRF Connect for PC v1.1.1.
        // 7500 ms is the smallest stable value with nRF Connect for Android v1.1.1.
        // Smaller values may allow the device to reset before the next DFU transation is started.
        err_code = app_timer_start(nrf_dfu_post_sd_bl_timeout_timer_id, APP_TIMER_TICKS(NRF_DFU_POST_SD_BL_TIMEOUT_MS), NULL);
        APP_ERROR_CHECK(err_code);
    }
}


/** @brief Function to execute the continuation of a SoftDevice update.
 *
 * @param[in]       src_addr            Source address of the SoftDevice to copy from.
 * @param[in]       p_bank              Pointer to the bank where the SoftDevice resides.
 *
 * @retval NRF_SUCCESS Continuation was successful.
 * @retval NRF_ERROR_INVALID_LENGTH Invalid length.
 * @retval NRF_ERROR_NO_MEM If UICR.NRFFW[1] is not set (i.e. is 0xFFFFFFFF).
 * @retval NRF_ERROR_INVALID_PARAM If an invalid command is given.
 * @retval NRF_ERROR_INTERNAL Indicates that the contents of the memory blocks were not verified correctly after copying.
 * @retval NRF_ERROR_NULL If the content of the memory blocks differs after copying.
 */
#if defined(SOFTDEVICE_PRESENT)
static uint32_t nrf_dfu_sd_continue_impl(uint32_t             src_addr,
                                         nrf_dfu_bank_t     * p_bank)
{
    uint32_t   ret_val      = NRF_SUCCESS;
    uint32_t   target_addr  = SOFTDEVICE_REGION_START + s_dfu_settings.write_offset;
    uint32_t   length_left  = align_to_page(s_dfu_settings.sd_size - s_dfu_settings.write_offset);
    uint32_t   split_size   = align_to_page(length_left / 4);

    NRF_LOG_DEBUG("Enter nrf_bootloader_dfu_sd_continue");

    // This can be a continuation due to a power failure
    src_addr += s_dfu_settings.write_offset;

    if (s_dfu_settings.sd_size != 0 && s_dfu_settings.write_offset == s_dfu_settings.sd_size)
    {
        NRF_LOG_DEBUG("SD already copied");
        return NRF_SUCCESS;
    }

    if (s_dfu_settings.write_offset == 0)
    {
        NRF_LOG_DEBUG("Updating SD. Old SD ver: %d, New ver: %d",
            SD_VERSION_GET(MBR_SIZE) / 100000, SD_VERSION_GET(src_addr) / 100000);
    }

    do
    {
        // If less than split size remain, reduce split size to avoid overwriting Bank 0.
        if (split_size > length_left)
        {
            split_size = align_to_page(length_left);
        }

        NRF_LOG_DEBUG("Copying [0x%08x-0x%08x] to [0x%08x-0x%08x]: Len: 0x%08x", src_addr, src_addr + split_size, target_addr, target_addr + split_size, split_size);

        // Copy a chunk of the SD. Size in words
        ret_val = nrf_dfu_mbr_copy_sd((uint32_t*)target_addr, (uint32_t*)src_addr, split_size);
        if (ret_val != NRF_SUCCESS)
        {
            NRF_LOG_ERROR("Failed to copy SD: target: 0x%08x, src: 0x%08x, len: 0x%08x", target_addr, src_addr, split_size);
            return ret_val;
        }

        NRF_LOG_DEBUG("Finished copying [0x%08x-0x%08x] to [0x%08x-0x%08x]: Len: 0x%08x", src_addr, src_addr + split_size, target_addr, target_addr + split_size, split_size);

        // Validate copy. Size in words
        ret_val = nrf_dfu_mbr_compare((uint32_t*)target_addr, (uint32_t*)src_addr, split_size);
        if (ret_val != NRF_SUCCESS)
        {
            NRF_LOG_ERROR("Failed to Compare SD: target: 0x%08x, src: 0x%08x, len: 0x%08x", target_addr, src_addr, split_size);
            return ret_val;
        }

        NRF_LOG_DEBUG("Validated 0x%08x-0x%08x to 0x%08x-0x%08x: Size: 0x%08x", src_addr, src_addr + split_size, target_addr, target_addr + split_size, split_size);

        target_addr += split_size;
        src_addr += split_size;

        if (split_size > length_left)
        {
            length_left = 0;
        }
        else
        {
            length_left -= split_size;
        }

        NRF_LOG_DEBUG("Finished with the SD update.");

        // Save the updated point of writes in case of power loss
        s_dfu_settings.write_offset = s_dfu_settings.sd_size - length_left;
        ret_val = nrf_dfu_settings_write(NULL);
    }
    while (length_left > 0);

    return ret_val;
}


/** @brief Function to continue SoftDevice update.
 *
 * @details     This function will be called after reset if there is a valid SoftDevice in Bank0 or Bank1
 *          required to be relocated and activated through MBR commands.
 *
 * @param[in]       src_addr            Source address of the SoftDevice to copy from.
 * @param[in]       p_bank              Pointer to the bank where the SoftDevice resides.
 *
 * @retval NRF_SUCCESS Continuation was successful.
 * @retval NRF_ERROR_INVALID_LENGTH Invalid length.
 * @retval NRF_ERROR_NO_MEM If UICR.NRFFW[1] is not set (i.e. is 0xFFFFFFFF).
 * @retval NRF_ERROR_INVALID_PARAM If an invalid command is given.
 * @retval NRF_ERROR_INTERNAL Indicates that the contents of the memory blocks were not verified correctly after copying.
 * @retval NRF_ERROR_NULL If the content of the memory blocks differs after copying.
 */
static uint32_t nrf_dfu_sd_continue(uint32_t             src_addr,
                                    nrf_dfu_bank_t     * p_bank)
{
    uint32_t ret_val;

    ret_val = nrf_dfu_sd_continue_impl(src_addr, p_bank);
    if (ret_val != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("SD update continuation failed");
        return ret_val;
    }

    nrf_dfu_invalidate_bank(p_bank);

    // Upon successful completion, the callback function will be called and reset the device. If a valid app is present, it will launch.
    NRF_LOG_DEBUG("Writing settings and reseting device.");
    ret_val = nrf_dfu_settings_write(reset_device_callback);

    return ret_val;
}
#endif

/** @brief Function to continue bootloader update.
 *
 * @details     This function will be called after reset if there is a valid bootloader in Bank 0 or Bank 1
 *              required to be relocated and activated through MBR commands.
 *
 * @param[in]       src_addr            Source address of the BL to copy from.
 * @param[in]       p_bank              Pointer to the bank where the SoftDevice resides.
 *
 * @return This function will not return if the bootloader is copied successfully.
 *         After the copy is verified, the device will reset and start the new bootloader.
 *
 * @retval NRF_SUCCESS Continuation was successful.
 * @retval NRF_ERROR_INVALID_LENGTH Invalid length of flash operation.
 * @retval NRF_ERROR_NO_MEM If no parameter page is provided (see sds for more info).
 * @retval NRF_ERROR_INVALID_PARAM If an invalid command is given.
 * @retval NRF_ERROR_INTERNAL Internal error that should not happen.
 * @retval NRF_ERROR_FORBIDDEN If NRF_UICR->BOOTADDR is not set.
 */
static uint32_t nrf_dfu_bl_continue(uint32_t src_addr, nrf_dfu_bank_t * p_bank)
{
    uint32_t        ret_val     = NRF_SUCCESS;
    uint32_t const  len         = (p_bank->image_size - s_dfu_settings.sd_size);

    // if the update is a combination of BL + SD, offset with SD size to get BL start address
    src_addr += s_dfu_settings.sd_size;

    NRF_LOG_DEBUG("Verifying BL: Addr: 0x%08x, Src: 0x%08x, Len: 0x%08x", MAIN_APPLICATION_START_ADDR, src_addr, len);

    // If the bootloader is the same as the banked version, the copy is finished
    ret_val = nrf_dfu_mbr_compare((uint32_t*)BOOTLOADER_START_ADDR, (uint32_t*)src_addr, len);
    if (ret_val == NRF_SUCCESS)
    {
        NRF_LOG_DEBUG("Bootloader was verified");

        // Invalidate bank, marking completion
        nrf_dfu_invalidate_bank(p_bank);

        // Upon successful completion, the callback function will be called and reset the device. If a valid app is present, it will launch.
        NRF_LOG_DEBUG("Writing settings and reseting device.");
        ret_val = nrf_dfu_settings_write(reset_device_callback);
    }
    else
    {
        NRF_LOG_DEBUG("Bootloader not verified, copying: Src: 0x%08x, Len: 0x%08x", src_addr, len);
        // Bootloader is different than the banked version. Continue copy
        // Note that if the SD and BL was combined, then the split point between them is in s_dfu_settings.sd_size
        ret_val = nrf_dfu_mbr_copy_bl((uint32_t*)src_addr, len);
        if (ret_val != NRF_SUCCESS)
        {
            NRF_LOG_ERROR("Request to copy BL failed");
        }
    }

    return ret_val;
}


/** @brief Function to continue combined bootloader and SoftDevice update.
 *
 * @details     This function will be called after reset if there is a valid bootloader and SoftDevice in Bank 0 or Bank 1
 *              required to be relocated and activated through MBR commands.
 *
 * @param[in]       src_addr            Source address of the combined bootloader and SoftDevice to copy from.
 * @param[in]       p_bank              Pointer to the bank where the SoftDevice resides.
 *
 * @retval NRF_SUCCESS Continuation was successful.
 * @retval NRF_ERROR_INVALID_LENGTH Invalid length.
 * @retval NRF_ERROR_NO_MEM If UICR.NRFFW[1] is not set (i.e. is 0xFFFFFFFF).
 * @retval NRF_ERROR_INVALID_PARAM If an invalid command is given.
 * @retval NRF_ERROR_INTERNAL Indicates that the contents of the memory blocks where not verified correctly after copying.
 * @retval NRF_ERROR_NULL If the content of the memory blocks differs after copying.
 * @retval NRF_ERROR_FORBIDDEN If NRF_UICR->BOOTADDR is not set.
 */
#if defined(SOFTDEVICE_PRESENT)
static uint32_t nrf_dfu_sd_bl_continue(uint32_t src_addr, nrf_dfu_bank_t * p_bank)
{
    uint32_t ret_val = NRF_SUCCESS;

    NRF_LOG_DEBUG("Enter nrf_dfu_sd_bl_continue");

    ret_val = nrf_dfu_sd_continue_impl(src_addr, p_bank);
    if (ret_val != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("SD+BL: SD copy failed");
        return ret_val;
    }

    ret_val = nrf_dfu_bl_continue(src_addr, p_bank);
    if (ret_val != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("SD+BL: BL copy failed");
        return ret_val;
    }

    return ret_val;
}
#endif

static uint32_t nrf_dfu_continue_bank(nrf_dfu_bank_t * p_bank, uint32_t src_addr, uint32_t * p_enter_dfu_mode)
{
    uint32_t ret_val = NRF_SUCCESS;

    switch (p_bank->bank_code)
    {
       case NRF_DFU_BANK_VALID_APP:
            NRF_LOG_DEBUG("Valid App");
            if(s_dfu_settings.bank_current == NRF_DFU_CURRENT_BANK_1)
            {
                // Only continue copying if valid app in Bank 1
                ret_val = nrf_dfu_app_continue(src_addr);
            }
            break;
#if defined(SOFTDEVICE_PRESENT)
       case NRF_DFU_BANK_VALID_SD:
            NRF_LOG_DEBUG("Valid SD");
            // There is a valid SD that needs to be copied (or continued)
            ret_val = nrf_dfu_sd_continue(src_addr, p_bank);
            (*p_enter_dfu_mode) = 1;
            break;
#endif

        case NRF_DFU_BANK_VALID_BL:
            NRF_LOG_DEBUG("Valid BL");
            // There is a valid BL that must be copied (or continued)
            ret_val = nrf_dfu_bl_continue(src_addr, p_bank);
            break;

#if defined(SOFTDEVICE_PRESENT)
        case NRF_DFU_BANK_VALID_SD_BL:
            NRF_LOG_DEBUG("Valid SD + BL");
            // There is a valid SD + BL that must be copied (or continued)
            ret_val = nrf_dfu_sd_bl_continue(src_addr, p_bank);
            (*p_enter_dfu_mode) = 1;
            // Set the bank-code to invalid, and reset size/CRC
            break;
#endif

        case NRF_DFU_BANK_INVALID:
        default:
            NRF_LOG_ERROR("Single: Invalid bank");
            break;
    }

    return ret_val;
}


void nrf_dfu_reset_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);

    NRF_LOG_DEBUG("DFU reset due to inactivity timeout.");

#ifdef NRF_DFU_DEBUG_VERSION
    NRF_LOG_FLUSH();
    nrf_delay_ms(100);
#endif

    NVIC_SystemReset();
}


uint32_t nrf_dfu_continue(uint32_t * p_enter_dfu_mode)
{
    uint32_t            ret_val;
    nrf_dfu_bank_t    * p_bank;
    uint32_t            src_addr = CODE_REGION_1_START;

    NRF_LOG_DEBUG("Enter nrf_dfu_continue");

    if (s_dfu_settings.bank_layout == NRF_DFU_BANK_LAYOUT_SINGLE )
    {
        p_bank = &s_dfu_settings.bank_0;
    }
    else if(s_dfu_settings.bank_current == NRF_DFU_CURRENT_BANK_0)
    {
        p_bank = &s_dfu_settings.bank_0;
    }
    else
    {
        p_bank = &s_dfu_settings.bank_1;
        src_addr += align_to_page(s_dfu_settings.bank_0.image_size);
    }

    ret_val = nrf_dfu_continue_bank(p_bank, src_addr, p_enter_dfu_mode);
    return ret_val;
}


bool nrf_dfu_app_is_valid(void)
{
    NRF_LOG_DEBUG("Enter nrf_dfu_app_is_valid");
    if (s_dfu_settings.bank_0.bank_code != NRF_DFU_BANK_VALID_APP)
    {
       // Bank 0 has no valid app. Nothing to boot
       NRF_LOG_DEBUG("Return false in valid app check");
       return false;
    }

    // If CRC == 0, the CRC check is skipped.
    if (s_dfu_settings.bank_0.image_crc != 0)
    {
        uint32_t crc = crc32_compute((uint8_t*) CODE_REGION_1_START,
                                     s_dfu_settings.bank_0.image_size,
                                     NULL);

        if (crc != s_dfu_settings.bank_0.image_crc)
        {
            // CRC does not match with what is stored.
            NRF_LOG_DEBUG("Return false in CRC");
            return  false;
        }
    }

    NRF_LOG_DEBUG("Return true. App was valid");
    return true;
}


uint32_t nrf_dfu_find_cache(uint32_t size_req, bool dual_bank_only, uint32_t * p_address)
{
    uint32_t free_size =  DFU_REGION_TOTAL_SIZE - DFU_APP_DATA_RESERVED;
    nrf_dfu_bank_t * p_bank;

    NRF_LOG_DEBUG("Enter nrf_dfu_find_cache");

    if (p_address == NULL)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    // Simple check whether the size requirement can be met
    if(free_size < size_req)
    {
        NRF_LOG_DEBUG("No way to fit the new firmware on device");
        return NRF_ERROR_NO_MEM;
    }

    NRF_LOG_DEBUG("Bank content");
    NRF_LOG_DEBUG("Bank type: %d", s_dfu_settings.bank_layout);
    NRF_LOG_DEBUG("Bank 0 code: 0x%02x: Size: %d", s_dfu_settings.bank_0.bank_code, s_dfu_settings.bank_0.image_size);
    NRF_LOG_DEBUG("Bank 1 code: 0x%02x: Size: %d", s_dfu_settings.bank_1.bank_code, s_dfu_settings.bank_1.image_size);

    // Setting Bank 0 as candidate
    p_bank = &s_dfu_settings.bank_0;

    // Setting candidate address
    (*p_address) = MAIN_APPLICATION_START_ADDR;

    // Calculate free size
    if (s_dfu_settings.bank_0.bank_code == NRF_DFU_BANK_VALID_APP)
    {
        // Valid app present.

        NRF_LOG_DEBUG("free_size before bank select: %d", free_size);

        free_size -= align_to_page(p_bank->image_size);

        NRF_LOG_DEBUG("free_size: %d, size_req: %d", free_size, size_req);

        // Check if we can fit the new app in the free space or if removal of old app is required.
        if (size_req > free_size)
        {
            // Not enough room in free space (Bank 1)
            if (dual_bank_only)
            {
                NRF_LOG_ERROR("Failure: dual bank restriction");
                return NRF_ERROR_NO_MEM;
            }

            // Can only support single bank update, clearing old app.
            s_dfu_settings.bank_layout = NRF_DFU_BANK_LAYOUT_SINGLE;
            s_dfu_settings.bank_current = NRF_DFU_CURRENT_BANK_0;
            p_bank = &s_dfu_settings.bank_0;
            NRF_LOG_DEBUG("Enforcing single bank");
        }
        else
        {
            // Room in Bank 1 for update
            // Ensure we are using dual bank layout
            s_dfu_settings.bank_layout = NRF_DFU_BANK_LAYOUT_DUAL;
            s_dfu_settings.bank_current = NRF_DFU_CURRENT_BANK_1;
            p_bank = &s_dfu_settings.bank_1;
            // Set to first free page boundary after previous app
            (*p_address) += align_to_page(s_dfu_settings.bank_0.image_size);
            NRF_LOG_DEBUG("Using second bank");
        }
    }
    else
    {
        // No valid app present. Promoting dual bank.
        s_dfu_settings.bank_layout  = NRF_DFU_BANK_LAYOUT_DUAL;
        s_dfu_settings.bank_current = NRF_DFU_CURRENT_BANK_0;

        p_bank = &s_dfu_settings.bank_0;
        NRF_LOG_DEBUG("No previous, using bank 0");
    }

    // Set the bank-code to invalid, and reset size/CRC
    memset(p_bank, 0, sizeof(nrf_dfu_bank_t));

    // Store the Firmware size in the bank for continuations
    p_bank->image_size = size_req;
    return NRF_SUCCESS;
}

