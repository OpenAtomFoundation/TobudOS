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

/** @file
 * @defgroup qspi_bootloader_example_main main.c
 * @{
 * @ingroup qspi_bootloader_example
 *
 * @brief QSPI Bootloader Example Application main file.
 *
 * This file contains the source code for a sample bootloader starting application from QSPI.
 */

#include <nrfx_qspi.h>
#include <boards.h>

#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <nrf_log_default_backends.h>

#define QSPI_STD_CMD_WRSR        0x01
#define QSPI_STD_CMD_RSTEN       0x66
#define QSPI_STD_CMD_RST         0x99

#define QSPI_SR_QUAD_ENABLE_BYTE 0x40

#define QSPI_XIP_START_ADDR      0x12000000

static void configure_memory(void)
{
    uint32_t err_code;
    uint8_t data;
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = QSPI_STD_CMD_RSTEN,
        .length    = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = true,
        .io3_level = true,
        .wipwait   = true,
        .wren      = true
    };

    // Send reset enable
    err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Send reset command
    cinstr_cfg.opcode = QSPI_STD_CMD_RST;
    err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Switch to qspi mode
    data = QSPI_SR_QUAD_ENABLE_BYTE;
    cinstr_cfg.opcode = QSPI_STD_CMD_WRSR;
    cinstr_cfg.length = NRF_QSPI_CINSTR_LEN_2B;
    err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, &data, NULL);
    APP_ERROR_CHECK(err_code);

    while (nrfx_qspi_mem_busy_check())
    {}
}

int main(void)
{
    uint32_t err_code = NRF_SUCCESS;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    // Set QSPI peripheral with default configuration.
    nrfx_qspi_config_t config = NRFX_QSPI_DEFAULT_CONFIG;

    // Set QSPI pins to pins related to connected board.
    config.pins.sck_pin = BSP_QSPI_SCK_PIN;
    config.pins.csn_pin = BSP_QSPI_CSN_PIN;
    config.pins.io0_pin = BSP_QSPI_IO0_PIN;
    config.pins.io1_pin = BSP_QSPI_IO1_PIN;
    config.pins.io2_pin = BSP_QSPI_IO2_PIN;
    config.pins.io3_pin = BSP_QSPI_IO3_PIN;

    // Try to initialize QSPI peripheral in blocking mode.
    err_code = nrfx_qspi_init(&config, NULL, NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("QSPI Bootloader started.");

    // Restart and configure external memory to use quad line mode for data exchange.
    configure_memory();
    NRF_LOG_INFO("Memory device configured. Quad Mode activated.");

    NRF_LOG_INFO("Jumping to address 0x%x", QSPI_XIP_START_ADDR);
    NRF_LOG_FLUSH();


    // This simple bootloader assumes that the stack pointer will not be changed. Remember to change
    // it before booting if your application requires it.

    // Read reset_handler address from QSPI address space.
    nrfx_irq_handler_t reset_handler = (nrfx_irq_handler_t)((volatile uint32_t const *)QSPI_XIP_START_ADDR)[1];

    // The bootloader changes default vector table to application one. Some applications may
    // require to change vector table to os-centric or stack-centric like in the SoftDevice.
    // See @ref sd_softdevice_vector_table_base_set function for more.
    //
    // In case of big and complex bootloader with situations to fire interrupts, remember to uninit
    // used peripherals or their interrupts to avoid jump to the interrupt handler during changing
    // interrupts vector table address. It can be achieved using NVIC- or core-related functions
    // like:
    //     __disable_irq();
    // Interrupts have to be reenabled during start of the application.

    SCB->VTOR = QSPI_XIP_START_ADDR;

    // Boot the main application.
    reset_handler();
}

/** @} */
