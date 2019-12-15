/**
 * @file    wm_reflector.h
 *
 * @brief   reflector driver module
 *
 * @author  dave
 *
 * Copyright (c) 2016 Winner Microelectronics Co., Ltd.
 */

#ifndef WM_REFLECTOR_H
#define WM_REFLECTOR_H

#include "wm_type_def.h"

/**
 * @brief          This function is used to initial the reflector.
 *
 * @param[in]      ctr_io     ctrl io
 *
 * @return         None
 *
 * @note           None
 */
void tls_reflector_open(int ctr_io);

/**
 * @brief          This function is used to close the reflector.
 *
 * @param[in]      ctr_io    ctrl io
 *
 * @return         None
 *
 * @note           None
 */
void tls_reflector_close(int ctr_io);

/**
 * @brief          This function is used to read reflector output status.
 *
 * @param[in]      out_io    output io
 *
 * @retval         output status
 *
 * @note           None
 */
int tls_reflector_output_status(int out_io);

#endif

