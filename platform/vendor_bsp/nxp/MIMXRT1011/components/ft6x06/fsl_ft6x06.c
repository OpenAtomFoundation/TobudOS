/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_ft6x06.h"

typedef struct _ft6x06_touch_point
{
    uint8_t XH;
    uint8_t XL;
    uint8_t YH;
    uint8_t YL;
    uint8_t WEIGHT;
    uint8_t MISC;
} ft6x06_touch_point_t;

typedef struct _ft6x06_touch_data
{
    uint8_t GEST_ID;
    uint8_t TD_STATUS;
    ft6x06_touch_point_t TOUCH[FT6X06_MAX_TOUCHES];
} ft6x06_touch_data_t;

#define TOUCH_POINT_GET_EVENT(T) ((touch_event_t)((T).XH >> 6))
#define TOUCH_POINT_GET_ID(T) ((T).YH >> 4)
#define TOUCH_POINT_GET_X(T) ((((T).XH & 0x0f) << 8) | (T).XL)
#define TOUCH_POINT_GET_Y(T) ((((T).YH & 0x0f) << 8) | (T).YL)

void FT6X06_EventHandler(ft6x06_handle_t *handle, uint32_t i2c_event)
{
    handle->i2c_event          = i2c_event;
    handle->i2c_event_received = true;
}

static uint32_t FT_6X06_WaitEvent(ft6x06_handle_t *handle)
{
    uint32_t i2c_event;

    while (!(handle->i2c_event_received))
        ;

    i2c_event                  = handle->i2c_event;
    handle->i2c_event_received = false;

    return i2c_event;
}

status_t FT6X06_Init(ft6x06_handle_t *handle, ARM_DRIVER_I2C *i2c_driver)
{
    status_t status = kStatus_Success;
    uint8_t i2c_buf[2];

    assert(handle);
    assert(i2c_driver);

    if (!handle || !i2c_driver)
    {
        return kStatus_InvalidArgument;
    }

    handle->i2c_driver = i2c_driver;

    /* clear transfer structure and buffer */
    memset(handle->touch_buf, 0, FT6X06_TOUCH_DATA_LEN);

    /* set device mode to normal operation */
    i2c_buf[0] = 0; /* mode register address */
    i2c_buf[1] = 0; /* normal operation mode */

    if (handle->i2c_driver->MasterTransmit(FT6X06_I2C_ADDRESS, i2c_buf, 2, false) != ARM_DRIVER_OK)
    {
        status = kStatus_Fail;
    }
    else if (FT_6X06_WaitEvent(handle) != ARM_I2C_EVENT_TRANSFER_DONE)
    {
        status = kStatus_Fail;
    }

    return status;
}

status_t FT6X06_Denit(ft6x06_handle_t *handle)
{
    assert(handle);

    if (!handle)
    {
        return kStatus_InvalidArgument;
    }

    handle->i2c_driver = NULL;
    return kStatus_Success;
}

status_t FT6X06_ReadTouchData(ft6x06_handle_t *handle)
{
    status_t status = kStatus_Success;
    uint8_t i2c_buf[1];

    assert(handle);

    if (!handle || !(handle->i2c_driver))
    {
        return kStatus_InvalidArgument;
    }

    i2c_buf[0] = F6X06_TOUCH_DATA_SUBADDR;

    if (handle->i2c_driver->MasterTransmit(FT6X06_I2C_ADDRESS, i2c_buf, 1, true) != ARM_DRIVER_OK)
    {
        status = kStatus_Fail;
    }
    else if (FT_6X06_WaitEvent(handle) != ARM_I2C_EVENT_TRANSFER_DONE)
    {
        status = kStatus_Fail;
    }
    else if (handle->i2c_driver->MasterReceive(FT6X06_I2C_ADDRESS, handle->touch_buf, FT6X06_TOUCH_DATA_LEN, false) !=
             ARM_DRIVER_OK)
    {
        status = kStatus_Fail;
    }
    else if (FT_6X06_WaitEvent(handle) != ARM_I2C_EVENT_TRANSFER_DONE)
    {
        status = kStatus_Fail;
    }

    return status;
}

status_t FT6X06_GetSingleTouch(ft6x06_handle_t *handle, touch_event_t *touch_event, int *touch_x, int *touch_y)
{
    status_t status;
    touch_event_t touch_event_local;

    status = FT6X06_ReadTouchData(handle);

    if (status == kStatus_Success)
    {
        ft6x06_touch_data_t *touch_data = (ft6x06_touch_data_t *)(void *)(handle->touch_buf);

        if (touch_event == NULL)
        {
            touch_event = &touch_event_local;
        }
        *touch_event = TOUCH_POINT_GET_EVENT(touch_data->TOUCH[0]);

        /* Update coordinates only if there is touch detected */
        if ((*touch_event == kTouch_Down) || (*touch_event == kTouch_Contact))
        {
            if (touch_x)
            {
                *touch_x = TOUCH_POINT_GET_X(touch_data->TOUCH[0]);
            }
            if (touch_y)
            {
                *touch_y = TOUCH_POINT_GET_Y(touch_data->TOUCH[0]);
            }
        }
    }

    return status;
}

status_t FT6X06_GetMultiTouch(ft6x06_handle_t *handle, int *touch_count, touch_point_t touch_array[FT6X06_MAX_TOUCHES])
{
    status_t status;

    status = FT6X06_ReadTouchData(handle);

    if (status == kStatus_Success)
    {
        ft6x06_touch_data_t *touch_data = (ft6x06_touch_data_t *)(void *)(handle->touch_buf);
        int i;

        /* Decode number of touches */
        if (touch_count)
        {
            *touch_count = touch_data->TD_STATUS;
        }

        /* Decode valid touch points */
        for (i = 0; i < touch_data->TD_STATUS; i++)
        {
            touch_array[i].TOUCH_ID    = TOUCH_POINT_GET_ID(touch_data->TOUCH[i]);
            touch_array[i].TOUCH_EVENT = TOUCH_POINT_GET_EVENT(touch_data->TOUCH[i]);
            touch_array[i].TOUCH_X     = TOUCH_POINT_GET_X(touch_data->TOUCH[i]);
            touch_array[i].TOUCH_Y     = TOUCH_POINT_GET_Y(touch_data->TOUCH[i]);
        }

        /* Clear vacant elements of touch_array */
        for (; i < FT6X06_MAX_TOUCHES; i++)
        {
            touch_array[i].TOUCH_ID    = 0;
            touch_array[i].TOUCH_EVENT = kTouch_Reserved;
            touch_array[i].TOUCH_X     = 0;
            touch_array[i].TOUCH_Y     = 0;
        }
    }

    return status;
}
