/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file
*
* \brief This file implements the platform abstraction layer(pal) APIs for I2C.
*
* \addtogroup  grPAL
* @{
*/

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include "optiga/pal/pal_i2c.h"
#include "optiga/ifx_i2c/ifx_i2c.h"
#include "nrf_twi_mngr.h"
#include "boards.h"

#include <stdbool.h>

/// @cond hidden

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/
/** @brief Max bitrate for i2c master */
#define PAL_I2C_MASTER_MAX_BITRATE  (400)

/** @brief PIN for I2C SCL to Infineon OPTIGA Trust X device */
#define OPTIGA_PIN_I2C_SCL   (ARDUINO_SCL_PIN)
/** @brief PIN for I2C SDA to Infineon OPTIGA Trust X device */
#define OPTIGA_PIN_I2C_SDA   (ARDUINO_SDA_PIN)

/** @brief I2C driver instance */
#define TWI_INSTANCE_ID             0

/** @brief Maximal number of pending I2C transactions */
#define MAX_PENDING_TRANSACTIONS    5

/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/

/* Pointer to the current pal i2c context */
static pal_i2c_t * gp_pal_i2c_current_ctx;

/** @brief Definition of TWI manager instance */
#ifndef IFX_2GO_SUPPORT
NRF_TWI_MNGR_DEF(m_app_twi, MAX_PENDING_TRANSACTIONS, TWI_INSTANCE_ID);
#else
nrf_twi_mngr_t m_app_twi;
#endif

/** @brief Definition of TWI manager transfer instance */
static nrf_twi_mngr_transfer_t    m_transfer;

/** @brief Definition of TWI manager transaction instance */
static nrf_twi_mngr_transaction_t m_transaction;

static bool initialized = false;

/**********************************************************************************************************************
 * LOCAL ROUTINES
 *********************************************************************************************************************/

/**
 * Pal I2C event handler function to invoke the registered upper layer callback<br>
 *
 *<b>API Details:</b>
 *  - This function implements the platform specific i2c event handling mechanism<br>
 *  - It calls the registered upper layer function after completion of the I2C read/write operations<br>
 *  - The respective event status are explained below.
 *   - #PAL_I2C_EVENT_ERROR when I2C fails due to low level failures(NACK/I2C protocol errors)
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successfully completed
 *
 * \param[in] p_pal_i2c_ctx   Pointer to the pal i2c context #pal_i2c_t
 * \param[in] event           Status of the event reported after read/write completion or due to I2C errors
 *
 */
static void app_twi_callback(ret_code_t result, void * p_user_data)
{
    app_event_handler_t upper_layer_handler;
    //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
    upper_layer_handler = (app_event_handler_t)gp_pal_i2c_current_ctx->upper_layer_event_handler;

    if (result == NRF_SUCCESS)
    {
        upper_layer_handler(gp_pal_i2c_current_ctx->upper_layer_ctx, PAL_I2C_EVENT_SUCCESS);
    }
    else
    {
        upper_layer_handler(gp_pal_i2c_current_ctx->upper_layer_ctx, PAL_I2C_EVENT_ERROR);
    }
}

/// @endcond

/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/

pal_status_t pal_i2c_init(const pal_i2c_t* p_i2c_context)
{
#ifndef IFX_2GO_SUPPORT
    nrf_drv_twi_config_t const config = {
       .scl                = OPTIGA_PIN_I2C_SCL,
       .sda                = OPTIGA_PIN_I2C_SDA,
       .frequency          = NRF_DRV_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
       .clear_bus_init     = false
    };
#else
    #include "ifx_2go_common.h"
    nrf_drv_twi_config_t const config = {
       .scl                = ifx_2go_pin_config()->scl,
       .sda                = ifx_2go_pin_config()->sda,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
       .clear_bus_init     = false
    };
#endif

    if(initialized)
    {
        nrf_twi_mngr_uninit(&m_app_twi);
    }

    // Initialize I2C driver
    if (nrf_twi_mngr_init(&m_app_twi, &config) != NRF_SUCCESS)
    {
            return PAL_STATUS_FAILURE;
    }

    initialized = true;
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_deinit(const pal_i2c_t* p_i2c_context)
{
    if(initialized) {
        nrf_twi_mngr_uninit(&m_app_twi);
    }
    initialized = false;
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_write(pal_i2c_t* p_i2c_context,uint8_t* p_data , uint16_t length)
{
    gp_pal_i2c_current_ctx = p_i2c_context;

    m_transfer.p_data    = p_data;
    m_transfer.length    = length;
    m_transfer.operation = NRF_TWI_MNGR_WRITE_OP(IFX_I2C_BASE_ADDR);
    m_transfer.flags     = 0;

    m_transaction.callback            = app_twi_callback;
    m_transaction.number_of_transfers = 1;
    m_transaction.p_required_twi_cfg  = NULL;
    m_transaction.p_transfers         = &m_transfer;
    m_transaction.p_user_data         = (void*) PAL_STATUS_SUCCESS;

    if (nrf_twi_mngr_schedule(&m_app_twi, &m_transaction) != NRF_SUCCESS)
    {
        app_twi_callback(NRF_ERROR_BUSY, 0);
    }

    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_read(pal_i2c_t* p_i2c_context , uint8_t* p_data , uint16_t length)
{
    gp_pal_i2c_current_ctx = p_i2c_context;

    m_transfer.p_data    = p_data;
    m_transfer.length    = length;
    m_transfer.operation = NRF_TWI_MNGR_READ_OP(IFX_I2C_BASE_ADDR);
    m_transfer.flags     = 0;

    m_transaction.callback            = app_twi_callback;
    m_transaction.number_of_transfers = 1;
    m_transaction.p_required_twi_cfg  = 0;
    m_transaction.p_transfers         = &m_transfer;
    m_transaction.p_user_data         = (void*) PAL_STATUS_SUCCESS;

    if (nrf_twi_mngr_schedule(&m_app_twi, &m_transaction) != NRF_SUCCESS)
    {
        app_twi_callback(NRF_ERROR_BUSY, 0);
    }

    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_set_bitrate(const pal_i2c_t* p_i2c_context , uint16_t bitrate)
{
    // Bitrate is fixed to the maximum frequency on this platform (400K)
    return PAL_STATUS_SUCCESS;
}

#ifdef IFX_2GO_SUPPORT
pal_status_t pal_i2c_set_instance(nrf_twi_mngr_t* twi_inst)
{
        m_app_twi = *twi_inst;
}
#endif/*IFX_2GO_SUPPORT*/

/**
* @}
*/
