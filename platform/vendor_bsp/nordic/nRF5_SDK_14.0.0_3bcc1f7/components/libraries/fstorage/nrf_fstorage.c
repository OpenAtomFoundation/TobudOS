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
#include "sdk_config.h"

#if NRF_FSTORAGE_ENABLED

#include "nrf_fstorage.h"
#include <stddef.h>
#include <stdint.h>
#include "sdk_errors.h"
#include "nrf_section.h"


/* Create the section "fs_data". */
NRF_SECTION_DEF(fs_data, nrf_fstorage_t);

static bool addr_within_bounds(nrf_fstorage_t const * p_fs, uint32_t addr, uint32_t len);
static bool addr_is_aligned32(uint32_t addr);


ret_code_t nrf_fstorage_init(nrf_fstorage_t     * p_fs,
                             nrf_fstorage_api_t * p_api,
                             void               * p_param)
{
    if ((p_fs == NULL) || (p_api == NULL))
    {
        return NRF_ERROR_NULL;
    }

    p_fs->p_api = (struct nrf_fstorage_api_t *) p_api;

    return ((nrf_fstorage_api_t*)p_fs->p_api)->init(p_fs, p_param);
}


ret_code_t nrf_fstorage_uninit(nrf_fstorage_t * p_fs,
                               void           * p_param)
{
    ret_code_t rc;

    if (p_fs == NULL)
    {
        return NRF_ERROR_NULL;
    }

    if (p_fs->p_api == NULL)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    rc = ((nrf_fstorage_api_t *)p_fs->p_api)->uninit(p_fs, p_param);

    /* Uninitialize the API. */
    p_fs->p_api        = NULL;
    p_fs->p_flash_info = NULL;

    return rc;
}


ret_code_t nrf_fstorage_read(nrf_fstorage_t const * p_fs,
                             uint32_t               src,
                             void                 * p_dest,
                             uint32_t               len)
{
    if ((p_fs == NULL) || (p_dest == NULL))
    {
        return NRF_ERROR_NULL;
    }

    if (p_fs->p_api == NULL)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if (len == 0)
    {
        return NRF_ERROR_INVALID_LENGTH;
    }

    /* Source addres must be word-aligned. */
    if (    !addr_is_aligned32(src)
        ||  !addr_within_bounds(p_fs, src, len))
    {
        return NRF_ERROR_INVALID_ADDR;
    }

    return ((nrf_fstorage_api_t *)p_fs->p_api)->read(p_fs, src, p_dest, len);
}


ret_code_t nrf_fstorage_write(nrf_fstorage_t const * p_fs,
                              uint32_t               dest,
                              void           const * p_src,
                              uint32_t               len,
                              void                 * p_context)
{
    if ((p_fs == NULL) || (p_src == NULL))
    {
        return NRF_ERROR_NULL;
    }

    if (p_fs->p_api == NULL)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    /* Length must be a multiple of the program unit. */
    if ((len == 0) || ((len % p_fs->p_flash_info->program_unit) != 0))
    {
        return NRF_ERROR_INVALID_LENGTH;
    }

    /* Source and destination addresses must be word-aligned. */
    if (    !addr_is_aligned32(dest)
        ||  !addr_is_aligned32((uint32_t)p_src)
        ||  !addr_within_bounds(p_fs, dest, len))
    {
        return NRF_ERROR_INVALID_ADDR;
    }

    return ((nrf_fstorage_api_t *)p_fs->p_api)->write(p_fs, dest, p_src, len, p_context);
}


ret_code_t nrf_fstorage_erase(nrf_fstorage_t const * p_fs,
                              uint32_t               page_addr,
                              uint32_t               len,
                              void                 * p_context)
{
    if (p_fs == NULL)
    {
        return NRF_ERROR_NULL;
    }

    if (p_fs->p_api == NULL)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if (len == 0)
    {
        return NRF_ERROR_INVALID_LENGTH;
    }

    /* Address must be aligned to a page boundary. */
    if (    ((page_addr & (p_fs->p_flash_info->erase_unit - 1)) != 0)
        ||  !addr_within_bounds(p_fs, page_addr, (len * p_fs->p_flash_info->erase_unit)))
    {
        return NRF_ERROR_INVALID_ADDR;
    }

    return ((nrf_fstorage_api_t *)p_fs->p_api)->erase(p_fs, page_addr, len, p_context);
}


bool nrf_fstorage_is_busy(nrf_fstorage_t const * p_fs)
{
    nrf_fstorage_api_t const * p_api;

    /* If a NULL instance is provided, return true if any instance is busy.
     * Uninitialized instances are considered not busy. */
    if ((p_fs == NULL) || (p_fs->p_api == NULL))
    {
        for (uint32_t i = 0; i < NRF_FSTORAGE_INSTANCE_CNT; i++)
        {
            p_fs  = NRF_FSTORAGE_INSTANCE_GET(i);   /* cannot be NULL. */
            p_api = (nrf_fstorage_api_t*)p_fs->p_api;

            if (p_api != NULL)
            {
                /* p_api->is_busy() cannot be NULL. */
                if (p_api->is_busy(p_fs))
                {
                    return true;
                }
            }
        }

        return false;
    }

    p_api = (nrf_fstorage_api_t*)p_fs->p_api;

    return p_api->is_busy(p_fs);
}


static bool addr_within_bounds(nrf_fstorage_t const * p_fs,
                               uint32_t               addr,
                               uint32_t               len)
{
    return ((addr           >= p_fs->start_addr) &&
            (addr + len - 1 <= p_fs->end_addr));
}


static bool addr_is_aligned32(uint32_t addr)
{
    return !(addr & 0x03);
}


#endif // NRF_FSTORAGE_ENABLED
