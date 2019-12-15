/**
 * @file    wm_reflector.c
 *
 * @brief   reflector driver
 *
 * @author  dave
 *
 * Copyright (c) 2016 Winner Microelectronics Co., Ltd.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wm_regs.h"
#include "wm_gpio.h"

/**
 * @brief          This function is used to initial the reflector.
 *
 * @param[in]      ctr_io     ctrl io
 *
 * @return         None
 *
 * @note           None
 */
void tls_reflector_open(int ctr_io)
{
    tls_gpio_cfg((enum tls_io_name)ctr_io, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_write((enum tls_io_name)ctr_io, 1);
}

/**
 * @brief          This function is used to close the reflector.
 *
 * @param[in]      ctr_io    ctrl io
 *
 * @return         None
 *
 * @note           None
 */
void tls_reflector_close(int ctr_io)
{
    tls_gpio_cfg((enum tls_io_name)ctr_io, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_write((enum tls_io_name)ctr_io, 0);
}

/**
 * @brief          This function is used to read reflector output status.
 *
 * @param[in]      out_io    output io
 *
 * @retval         output status
 *
 * @note           None
 */
int tls_reflector_output_status(int out_io)
{
    tls_gpio_cfg((enum tls_io_name)out_io, WM_GPIO_DIR_INPUT, WM_GPIO_ATTR_FLOATING);
    return tls_gpio_read((enum tls_io_name)out_io);
}

