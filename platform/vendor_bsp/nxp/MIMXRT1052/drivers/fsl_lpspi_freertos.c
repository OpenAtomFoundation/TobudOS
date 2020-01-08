/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
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

#include "fsl_lpspi_freertos.h"

static void LPSPI_RTOS_Callback(LPSPI_Type *base, lpspi_master_handle_t *drv_handle, status_t status, void *userData)
{
    lpspi_rtos_handle_t *handle = (lpspi_rtos_handle_t *)userData;
    BaseType_t reschedule;

    xSemaphoreGiveFromISR(handle->event, &reschedule);
    portYIELD_FROM_ISR(reschedule);
}

status_t LPSPI_RTOS_Init(lpspi_rtos_handle_t *handle,
                         LPSPI_Type *base,
                         const lpspi_master_config_t *masterConfig,
                         uint32_t srcClock_Hz)
{
    if (handle == NULL)
    {
        return kStatus_InvalidArgument;
    }

    if (base == NULL)
    {
        return kStatus_InvalidArgument;
    }

    memset(handle, 0, sizeof(lpspi_rtos_handle_t));

    handle->mutex = xSemaphoreCreateMutex();
    if (handle->mutex == NULL)
    {
        return kStatus_Fail;
    }

    handle->event = xSemaphoreCreateBinary();
    if (handle->event == NULL)
    {
        vSemaphoreDelete(handle->mutex);
        return kStatus_Fail;
    }

    handle->base = base;

    LPSPI_MasterInit(handle->base, masterConfig, srcClock_Hz);
    LPSPI_MasterTransferCreateHandle(handle->base, &handle->drv_handle, LPSPI_RTOS_Callback, (void *)handle);

    return kStatus_Success;
}

status_t LPSPI_RTOS_Deinit(lpspi_rtos_handle_t *handle)
{
    LPSPI_Deinit(handle->base);
    vSemaphoreDelete(handle->event);
    vSemaphoreDelete(handle->mutex);

    return kStatus_Success;
}

status_t LPSPI_RTOS_Transfer(lpspi_rtos_handle_t *handle, lpspi_transfer_t *transfer)
{
    status_t status;

    /* Lock resource mutex */
    if (xSemaphoreTake(handle->mutex, portMAX_DELAY) != pdTRUE)
    {
        return kStatus_LPSPI_Busy;
    }

    status = LPSPI_MasterTransferNonBlocking(handle->base, &handle->drv_handle, transfer);
    if (status != kStatus_Success)
    {
        xSemaphoreGive(handle->mutex);
        return status;
    }

    /* Wait for transfer to finish */
    xSemaphoreTake(handle->event, portMAX_DELAY);

    /* Unlock resource mutex */
    xSemaphoreGive(handle->mutex);

    /* Return status captured by callback function */
    return handle->async_status;
}
