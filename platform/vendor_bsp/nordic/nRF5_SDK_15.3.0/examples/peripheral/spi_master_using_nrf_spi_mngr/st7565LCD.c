/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
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

#include "st7565LCD.h"
#include "app_util_platform.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_spi_mngr.h"
#include "boards.h"
#include "nordic_common.h"

#define ST7565_PIN_A0      ARDUINO_7_PIN
#define ST7565_PIN_RESET   ARDUINO_12_PIN

#define ST7565_CMD_SET_COLUMN_UPPER ((uint8_t)0x10)
#define ST7565_CMD_SET_COLUMN_LOWER ((uint8_t)0x00)
#define ST7565_CMD_RMW              ((uint8_t)0xE0)
#define ST7565_CMD_SELECT_PAGE      ((uint8_t)0xB0)
#define ST7565_CMD_VREG_2           ((uint8_t)0x22)
#define ST7565_CMD_POWER_ON         ((uint8_t)0x2F)
#define ST7565_CMD_START_LINE       ((uint8_t)0x40)
#define ST7565_CMD_CONTRAST_SET     ((uint8_t)0x81)
#define ST7565_CMD_SEGMENT_NORMAL   ((uint8_t)0xA0)
#define ST7565_CMD_BIAS1            ((uint8_t)0xA2)
#define ST7565_CMD_ST7565_NORMAL    ((uint8_t)0xA6)
#define ST7565_CMD_ST7565_REVERSE   ((uint8_t)0xA7)
#define ST7565_CMD_OFF              ((uint8_t)0xAE)
#define ST7565_CMD_ON               ((uint8_t)0xAF)
#define ST7565_CMD_COMMON_NORMAL    ((uint8_t)0xC0)
#define ST7565_CMD_RESET            ((uint8_t)0xE2)
#define ST7565_CONTRAST_VALUE       ((uint8_t)0x17)

#define ST7565_PAGE_PIXELS_HEIGHT   ((uint8_t)8)
#define ST7565_DISPLAY_CMD_LENGTH   ((uint8_t)5)
#define ST7565_NUMBER_OF_PAGES      (ST7565_LCD_HEIGHT / ST7565_PAGE_PIXELS_HEIGHT)

#define ST7565_QUEUE_LENGTH     15
#define ST7565_SPI_INSTANCE_ID  0

NRF_SPI_MNGR_DEF(m_nrf_spi_mngr, ST7565_QUEUE_LENGTH, ST7565_SPI_INSTANCE_ID);

// Buffer for monochromatic display based on ST7565 driver.
// Each bit of a buffer corresponds to different pixel on a screen.
static uint8_t m_display_buffer[ST7565_LCD_WIDTH * ST7565_NUMBER_OF_PAGES];

static const uint8_t m_column_lookup[] = {7, 6, 5, 4, 3, 2, 1, 0};

// Buffer with commands used to display m_display_buffer on a screen.
static NRF_SPI_MNGR_BUFFER_LOC_IND uint8_t m_command_buffer[] =
{
    ST7565_CMD_SELECT_PAGE | 3,     // top page of the the screen
    ST7565_CMD_START_LINE,          // start with y = 0
    ST7565_CMD_SET_COLUMN_LOWER,    // start with x = 0
    ST7565_CMD_SET_COLUMN_UPPER,
    ST7565_CMD_RMW,                 // increment column addr at write

    ST7565_CMD_SELECT_PAGE | 2,
    ST7565_CMD_START_LINE,
    ST7565_CMD_SET_COLUMN_LOWER,
    ST7565_CMD_SET_COLUMN_UPPER,
    ST7565_CMD_RMW,

    ST7565_CMD_SELECT_PAGE | 1,
    ST7565_CMD_START_LINE,
    ST7565_CMD_SET_COLUMN_LOWER,
    ST7565_CMD_SET_COLUMN_UPPER,
    ST7565_CMD_RMW,

    ST7565_CMD_SELECT_PAGE,         // bottom page of the screen
    ST7565_CMD_START_LINE,
    ST7565_CMD_SET_COLUMN_LOWER,
    ST7565_CMD_SET_COLUMN_UPPER,
    ST7565_CMD_RMW,
};

// Initialization commands for ST7565 driver.
static NRF_SPI_MNGR_BUFFER_LOC_IND uint8_t m_initialization[] =
{
    ST7565_CMD_RESET,
    ST7565_CMD_OFF,
    ST7565_CMD_BIAS1,
    ST7565_CMD_SEGMENT_NORMAL,
    ST7565_CMD_COMMON_NORMAL,
    ST7565_CMD_VREG_2,
    ST7565_CMD_POWER_ON,
    ST7565_CMD_START_LINE,
    ST7565_CMD_ON,
    ST7565_CMD_CONTRAST_SET,
    ST7565_CONTRAST_VALUE,
    ST7565_CMD_ST7565_NORMAL
};


// Function called before SPI command is send to LCD display. I is used to drive A0 pin low.
static void st7565_set_command_cb(void * p_user_data)
{
    nrf_gpio_pin_clear(ST7565_PIN_A0);
}


// Function called before SPI data is send to LCD display. It is used to drive A0 pin high.
static void st7565_set_data_cb(void * p_user_data)
{
    nrf_gpio_pin_set(ST7565_PIN_A0);
}


// Function for displaying one of four pages on the LCD screen from m_display_buffer
static ret_code_t display_page(uint16_t page)
{
    ret_code_t ret_val;

    if (page >= ST7565_NUMBER_OF_PAGES)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    // Below structures have to be "static" - they cannot be placed on stack
    // since the transaction is scheduled and these structures most likely
    // will be referred after this function returns
    static nrf_spi_mngr_transfer_t NRF_SPI_MNGR_BUFFER_LOC_IND transfer_cmd[] =
    {
        NRF_SPI_MNGR_TRANSFER(&m_command_buffer[ST7565_DISPLAY_CMD_LENGTH * 0], // page 0
                              ST7565_DISPLAY_CMD_LENGTH, NULL, 0),
        NRF_SPI_MNGR_TRANSFER(&m_command_buffer[ST7565_DISPLAY_CMD_LENGTH * 1], // page 1
                              ST7565_DISPLAY_CMD_LENGTH, NULL, 0),
        NRF_SPI_MNGR_TRANSFER(&m_command_buffer[ST7565_DISPLAY_CMD_LENGTH * 2], // page 2
                              ST7565_DISPLAY_CMD_LENGTH, NULL, 0),
        NRF_SPI_MNGR_TRANSFER(&m_command_buffer[ST7565_DISPLAY_CMD_LENGTH * 3], // page 3
                              ST7565_DISPLAY_CMD_LENGTH, NULL, 0)
    };
    static nrf_spi_mngr_transaction_t const transaction_cmd[] =
    {
        {
            .begin_callback      = st7565_set_command_cb,
            .end_callback        = NULL,
            .p_user_data         = NULL,
            .p_transfers         = &transfer_cmd[0],    // page 0
            .number_of_transfers = 1,
            .p_required_spi_cfg  = NULL
        },
        {
            .begin_callback      = st7565_set_command_cb,
            .end_callback        = NULL,
            .p_user_data         = NULL,
            .p_transfers         = &transfer_cmd[1],    // page 1
            .number_of_transfers = 1,
            .p_required_spi_cfg  = NULL
        },
        {
            .begin_callback      = st7565_set_command_cb,
            .end_callback        = NULL,
            .p_user_data         = NULL,
            .p_transfers         = &transfer_cmd[2],    // page 2
            .number_of_transfers = 1,
            .p_required_spi_cfg  = NULL
        },
        {
            .begin_callback      = st7565_set_command_cb,
            .end_callback        = NULL,
            .p_user_data         = NULL,
            .p_transfers         = &transfer_cmd[3],    // page 3
            .number_of_transfers = 1,
            .p_required_spi_cfg  = NULL
        },
    };
    static nrf_spi_mngr_transfer_t NRF_SPI_MNGR_BUFFER_LOC_IND transfer_data[] =
    {
        NRF_SPI_MNGR_TRANSFER(&m_display_buffer[ST7565_LCD_WIDTH * 0], ST7565_LCD_WIDTH, NULL, 0),
        NRF_SPI_MNGR_TRANSFER(&m_display_buffer[ST7565_LCD_WIDTH * 1], ST7565_LCD_WIDTH, NULL, 0),
        NRF_SPI_MNGR_TRANSFER(&m_display_buffer[ST7565_LCD_WIDTH * 2], ST7565_LCD_WIDTH, NULL, 0),
        NRF_SPI_MNGR_TRANSFER(&m_display_buffer[ST7565_LCD_WIDTH * 3], ST7565_LCD_WIDTH, NULL, 0)
    };
    static nrf_spi_mngr_transaction_t const transaction_data[] =
    {
        {
            .begin_callback      = st7565_set_data_cb,
            .end_callback        = NULL,
            .p_user_data         = NULL,
            .p_transfers         = &transfer_data[0],   // page 0
            .number_of_transfers = 1,
            .p_required_spi_cfg  = NULL
        },
        {
            .begin_callback      = st7565_set_data_cb,
            .end_callback        = NULL,
            .p_user_data         = NULL,
            .p_transfers         = &transfer_data[1],   // page 1
            .number_of_transfers = 1,
            .p_required_spi_cfg  = NULL
        },
        {
            .begin_callback      = st7565_set_data_cb,
            .end_callback        = NULL,
            .p_user_data         = NULL,
            .p_transfers         = &transfer_data[2],  // page 2
            .number_of_transfers = 1,
            .p_required_spi_cfg  = NULL
        },
        {
            .begin_callback      = st7565_set_data_cb,
            .end_callback        = NULL,
            .p_user_data         = NULL,
            .p_transfers         = &transfer_data[3],  // page 3
            .number_of_transfers = 1,
            .p_required_spi_cfg  = NULL
        },
    };
    ret_val = nrf_spi_mngr_schedule(&m_nrf_spi_mngr, &transaction_cmd[page]);
    if (ret_val == NRF_SUCCESS)
    {
        ret_val = nrf_spi_mngr_schedule(&m_nrf_spi_mngr, &transaction_data[page]);
    }

    return ret_val;
}


static ret_code_t init_spi0_master(void)
{
    // SPI0 (with transaction manager) initialization.
    nrf_drv_spi_config_t const m_master0_config =
    {
        .sck_pin        = ARDUINO_13_PIN,
        .mosi_pin       = ARDUINO_11_PIN,
        .miso_pin       = NRF_DRV_SPI_PIN_NOT_USED,
        .ss_pin         = ARDUINO_10_PIN,
        .irq_priority   = APP_IRQ_PRIORITY_LOWEST,
        .orc            = 0xFF,
        .frequency      = NRF_DRV_SPI_FREQ_8M,
        .mode           = NRF_DRV_SPI_MODE_0,
        .bit_order      = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
    };
    return nrf_spi_mngr_init(&m_nrf_spi_mngr, &m_master0_config);
}


void st7565_clear_display(void)
{
    memset((void *)m_display_buffer, 0, sizeof(m_display_buffer));
    st7565_display_screen();
}


ret_code_t st7565_init(void)
{
    ret_code_t err_code;
    err_code = init_spi0_master();

    if (err_code == NRF_SUCCESS)
    {
        memset((void *)m_display_buffer, 0, sizeof(m_display_buffer));

        nrf_gpio_pin_dir_set(ST7565_PIN_A0, NRF_GPIO_PIN_DIR_OUTPUT);
        nrf_gpio_pin_dir_set(ST7565_PIN_RESET, NRF_GPIO_PIN_DIR_OUTPUT);

        nrf_gpio_pin_clear(ST7565_PIN_RESET);
        nrf_delay_ms(15);
        nrf_gpio_pin_set(ST7565_PIN_RESET);
        nrf_delay_ms(2);

        nrf_spi_mngr_transfer_t const transfers[] =
        {
            NRF_SPI_MNGR_TRANSFER(m_initialization, sizeof(m_initialization), NULL, 0)
        };
        err_code = nrf_spi_mngr_perform(&m_nrf_spi_mngr, NULL, transfers, ARRAY_SIZE(transfers), NULL);
    }

    return err_code;
}


void st7565_uninit(void)
{
    nrf_spi_mngr_uninit(&m_nrf_spi_mngr);
}


void st7565_pixel_draw(uint16_t x, uint16_t y, uint32_t color)
{
    ASSERT(x < ST7565_LCD_WIDTH);
    ASSERT(y < ST7565_LCD_HEIGHT);

    uint8_t page;
    uint8_t y_disp;
    uint8_t * p_byte;

    page = (uint8_t)(y >> 3);
    y_disp = m_column_lookup[y % 8];
    p_byte = &m_display_buffer[(ST7565_LCD_WIDTH - 1) * page + x + page];

    // In this example monochromatic LCD is used. Color > 0 sets pixel and color == 0 clears it.
    if (color != 0)
    {
        SET_BIT(*p_byte, y_disp);
    }
    else
    {
        CLR_BIT(*p_byte, y_disp);
    }
}


void st7565_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
    if ((x + width) >= ST7565_LCD_WIDTH)
    {
        return;
    }
    if ((y + height) >= ST7565_LCD_HEIGHT)
    {
        return;
    }

    for (uint8_t idx_y = y; idx_y < (y + height); idx_y++)
    {
        for (uint8_t idx_x = x; idx_x < (x + width); idx_x++)
        {
            st7565_pixel_draw(idx_x, idx_y, color);
        }
    }
}


void st7565_display_screen(void)
{
    for (uint8_t i = 0; i < ST7565_NUMBER_OF_PAGES; i++)
    {
        APP_ERROR_CHECK(display_page(i));
    }
}


void st7565_display_invert(bool invert)
{
    static uint8_t NRF_SPI_MNGR_BUFFER_LOC_IND command[] = {
                ST7565_CMD_ST7565_NORMAL, ST7565_CMD_ST7565_REVERSE
            };

    static nrf_spi_mngr_transfer_t const transfer_cmd[] =
    {
        NRF_SPI_MNGR_TRANSFER(&command[0], 1, NULL, 0),
        NRF_SPI_MNGR_TRANSFER(&command[1], 1, NULL, 0),
    };
    static nrf_spi_mngr_transaction_t const transaction_cmd[] =
    {
        {
            .begin_callback      = st7565_set_command_cb,
            .end_callback        = NULL,
            .p_user_data         = NULL,
            .p_transfers         = &transfer_cmd[0],
            .number_of_transfers = 1,
            .p_required_spi_cfg  = NULL
        },
        {
            .begin_callback      = st7565_set_command_cb,
            .end_callback        = NULL,
            .p_user_data         = NULL,
            .p_transfers         = &transfer_cmd[1],
            .number_of_transfers = 1,
            .p_required_spi_cfg  = NULL
        }
    };
    APP_ERROR_CHECK(nrf_spi_mngr_schedule(&m_nrf_spi_mngr, &transaction_cmd[invert]));
}

