/**
 * Copyright (c) 2017 - 2017, Nordic Semiconductor ASA
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
#include "nrf_ringbuf.h"
#include "app_util_platform.h"

#define WR_OFFSET 0
#define RD_OFFSET 1

void nrf_ringbuf_init(nrf_ringbuf_t const * p_ringbuf)
{
    p_ringbuf->p_cb->wr_idx = 0;
    p_ringbuf->p_cb->rd_idx = 0;
    p_ringbuf->p_cb->tmp_rd_idx = 0;
    p_ringbuf->p_cb->tmp_wr_idx = 0;
    p_ringbuf->p_cb->flag   = 0;
}

ret_code_t nrf_ringbuf_alloc(nrf_ringbuf_t const * p_ringbuf, uint8_t * * pp_data, size_t * p_length, bool start)
{
    if (start)
    {
        if (nrf_atomic_u32_fetch_add(&p_ringbuf->p_cb->flag, 1 << WR_OFFSET) & (1 << WR_OFFSET))
        {
            return NRF_ERROR_BUSY;
        }
    }

    if (p_ringbuf->p_cb->tmp_wr_idx - p_ringbuf->p_cb->rd_idx == p_ringbuf->bufsize_mask + 1)
    {
        *p_length = 0;
        return NRF_SUCCESS;
    }

    uint32_t wr_idx = p_ringbuf->p_cb->tmp_wr_idx & p_ringbuf->bufsize_mask;
    uint32_t rd_idx = p_ringbuf->p_cb->rd_idx & p_ringbuf->bufsize_mask;
    uint32_t available = (wr_idx >= rd_idx) ? p_ringbuf->bufsize_mask + 1 - wr_idx :
            p_ringbuf->p_cb->rd_idx -  p_ringbuf->p_cb->tmp_wr_idx;
    *p_length = *p_length < available ? *p_length : available;

    *pp_data = &p_ringbuf->p_buffer[wr_idx];
    p_ringbuf->p_cb->tmp_wr_idx += *p_length;

    return NRF_SUCCESS;
}

ret_code_t nrf_ringbuf_put(nrf_ringbuf_t const * p_ringbuf, size_t length)
{
    uint32_t available = p_ringbuf->bufsize_mask + 1 -
            (p_ringbuf->p_cb->wr_idx -  p_ringbuf->p_cb->rd_idx);
    if (length > available)
    {
        return NRF_ERROR_NO_MEM;
    }

    p_ringbuf->p_cb->wr_idx    += length;
    p_ringbuf->p_cb->tmp_wr_idx = p_ringbuf->p_cb->wr_idx;
    if (nrf_atomic_u32_fetch_sub(&p_ringbuf->p_cb->flag, 1 << WR_OFFSET) & (1 << WR_OFFSET))
    {
        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_STATE;
    }
}

ret_code_t nrf_ringbuf_cpy_put(nrf_ringbuf_t const * p_ringbuf,
                               uint8_t const * p_data,
                               size_t * p_length)
{
    CRITICAL_REGION_ENTER();
    uint32_t available = p_ringbuf->bufsize_mask + 1 -
                                (p_ringbuf->p_cb->wr_idx -  p_ringbuf->p_cb->rd_idx);
    *p_length = available > *p_length ? *p_length : available;
    size_t   length        = *p_length;
    uint32_t masked_wr_idx = (p_ringbuf->p_cb->wr_idx & p_ringbuf->bufsize_mask);
    uint32_t trail         = p_ringbuf->bufsize_mask + 1 - masked_wr_idx;

    if ( length > trail)
    {
        memcpy(&p_ringbuf->p_buffer[masked_wr_idx], p_data, trail);
        length -= trail;
        masked_wr_idx = 0;
        p_data += trail;
    }
    memcpy(&p_ringbuf->p_buffer[masked_wr_idx], p_data, length);
    p_ringbuf->p_cb->wr_idx += *p_length;
    CRITICAL_REGION_EXIT();

    return NRF_SUCCESS;
}

ret_code_t nrf_ringbuf_get(nrf_ringbuf_t const * p_ringbuf, uint8_t * * pp_data, size_t * p_length, bool start)
{
    if (start)
    {
        if (nrf_atomic_u32_fetch_add(&p_ringbuf->p_cb->flag, 1 << RD_OFFSET) & (1 << RD_OFFSET))
        {
            return NRF_ERROR_BUSY;
        }
    }

    uint32_t available = p_ringbuf->p_cb->wr_idx - p_ringbuf->p_cb->tmp_rd_idx;
    if (available == 0)
    {
        *p_length = 0;
        (void)nrf_atomic_u32_fetch_and(&p_ringbuf->p_cb->flag, ~(1 << RD_OFFSET));
        return NRF_SUCCESS;
    }

    uint32_t masked_tmp_rd_idx = p_ringbuf->p_cb->tmp_rd_idx & p_ringbuf->bufsize_mask;
    uint32_t masked_wr_idx     = p_ringbuf->p_cb->wr_idx & p_ringbuf->bufsize_mask;

    if ((masked_wr_idx > masked_tmp_rd_idx) && (available < *p_length))
    {
        *p_length = available;
    }
    else if (masked_wr_idx <= masked_tmp_rd_idx)
    {
        uint32_t trail = p_ringbuf->bufsize_mask + 1 - masked_tmp_rd_idx;
        if (*p_length > trail)
        {
            *p_length = trail;
        }
    }
    *pp_data = &p_ringbuf->p_buffer[masked_tmp_rd_idx];
    p_ringbuf->p_cb->tmp_rd_idx += *p_length;

    return NRF_SUCCESS;
}

ret_code_t nrf_ringbuf_cpy_get(nrf_ringbuf_t const * p_ringbuf,
                               uint8_t * p_data,
                               size_t * p_length)
{
    CRITICAL_REGION_ENTER();
    uint32_t available = p_ringbuf->p_cb->wr_idx -  p_ringbuf->p_cb->rd_idx;
    *p_length = available > *p_length ? *p_length : available;
    size_t   length        = *p_length;
    uint32_t masked_rd_idx = (p_ringbuf->p_cb->rd_idx & p_ringbuf->bufsize_mask);
    uint32_t masked_wr_idx = (p_ringbuf->p_cb->wr_idx & p_ringbuf->bufsize_mask);
    uint32_t trail         = (masked_wr_idx > masked_rd_idx) ? masked_wr_idx - masked_rd_idx :
                                                      p_ringbuf->bufsize_mask + 1 - masked_rd_idx;

    if ( length > trail)
    {
        memcpy(&p_ringbuf->p_buffer[masked_rd_idx], p_data, trail);
        length -= trail;
        masked_rd_idx = 0;
        p_data += trail;
    }
    memcpy(p_data, &p_ringbuf->p_buffer[masked_rd_idx], length);
    p_ringbuf->p_cb->rd_idx += *p_length;
    CRITICAL_REGION_EXIT();

    return NRF_SUCCESS;
}

ret_code_t nrf_ringbuf_free(nrf_ringbuf_t const * p_ringbuf, size_t length)
{
    uint32_t available = (p_ringbuf->p_cb->wr_idx -  p_ringbuf->p_cb->rd_idx);
    if (length > available)
    {
        return NRF_ERROR_NO_MEM;
    }

    p_ringbuf->p_cb->rd_idx    += length;
    p_ringbuf->p_cb->tmp_rd_idx = p_ringbuf->p_cb->rd_idx;
    (void)nrf_atomic_u32_fetch_and(&p_ringbuf->p_cb->flag, ~(1 << RD_OFFSET));
    return NRF_SUCCESS;
}

