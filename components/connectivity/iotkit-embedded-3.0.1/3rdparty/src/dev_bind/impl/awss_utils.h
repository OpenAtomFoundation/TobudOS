/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_UTILS_H__
#define __AWSS_UTILS_H__

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include "os.h"
#if 0
#include "iotx_utils.h"
#endif
#include "infra_string.h"

/**
 * @brief string to hex
 *
 * @param[in] str: input hex string
 * @param[in] str_len: length of input hex string
 * @param[out] out: output hex byte stream
 * @param[in/out] output_len: [in] for output buffer size, [out] for
 *                 output hex byte len
 * @Note None.
 *
 * @retval return num of hex bytes converted, 0 means error.
 */
#define utils_str_to_hex    LITE_hexstr_convert


/**
 * @brief hex to string
 *
 * @param[in] buf: input hex byte stream
 * @param[in] buf_len: input stream length in byte
 * @param[out] str: encoded hex string
 * @param[in/out] str_len: [in] for str buffer size, [out] for
 *                  encoded string length
 * @Note output str buffer is NULL-terminated(if str_buf_len is longer enough)
 *
 * @retval return length of str converted, 0 means error.
 */
#define utils_hex_to_str(buf, buf_len, str, str_buf_len) LITE_hexbuf_convert(buf, str, buf_len, 1)

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
