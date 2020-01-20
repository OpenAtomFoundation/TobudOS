/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_FT6X06_H_
#define _FSL_FT6X06_H_

#include "fsl_common.h"
#include "Driver_I2C.h"

/*!
 * @addtogroup ft6x06
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief FT6X06 I2C address. */
#define FT6X06_I2C_ADDRESS (0x38)

/*! @brief FT6X06 maximum number of simultaneously detected touches. */
#define FT6X06_MAX_TOUCHES (2U)

/*! @brief FT6X06 register address where touch data begin. */
#define F6X06_TOUCH_DATA_SUBADDR (1)

/*! @brief FT6X06 raw touch data length. */
#define FT6X06_TOUCH_DATA_LEN (2 + (FT6X06_MAX_TOUCHES)*6)

typedef enum _touch_event
{
    kTouch_Down     = 0, /*!< The state changed to touched. */
    kTouch_Up       = 1, /*!< The state changed to not touched. */
    kTouch_Contact  = 2, /*!< There is a continuous touch being detected. */
    kTouch_Reserved = 3  /*!< No touch information available. */
} touch_event_t;

typedef struct _touch_point
{
    touch_event_t TOUCH_EVENT; /*!< Indicates the state or event of the touch point. */
    uint8_t TOUCH_ID; /*!< Id of the touch point. This numeric value stays constant between down and up event. */
    uint16_t TOUCH_X; /*!< X coordinate of the touch point */
    uint16_t TOUCH_Y; /*!< Y coordinate of the touch point */
} touch_point_t;

typedef struct _ft6x06_handle
{
    ARM_DRIVER_I2C *i2c_driver;
    volatile uint32_t i2c_event;
    volatile bool i2c_event_received;
    uint8_t touch_buf[FT6X06_TOUCH_DATA_LEN];
} ft6x06_handle_t;

status_t FT6X06_Init(ft6x06_handle_t *handle, ARM_DRIVER_I2C *i2c_driver);

status_t FT6X06_Denit(ft6x06_handle_t *handle);

void FT6X06_EventHandler(ft6x06_handle_t *handle, uint32_t i2c_event);

status_t FT6X06_GetSingleTouch(ft6x06_handle_t *handle, touch_event_t *touch_event, int *touch_x, int *touch_y);

status_t FT6X06_GetMultiTouch(ft6x06_handle_t *handle, int *touch_count, touch_point_t touch_array[FT6X06_MAX_TOUCHES]);

#endif
