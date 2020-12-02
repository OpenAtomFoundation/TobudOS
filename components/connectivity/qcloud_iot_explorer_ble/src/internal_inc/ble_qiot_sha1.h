/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef QCLOUD_BLE_LLSYNC_BLE_QIOT_SHA1_H
#define QCLOUD_BLE_LLSYNC_BLE_QIOT_SHA1_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
 * \brief          SHA-1 context structure
 */
typedef struct {
    uint32_t      total[2];   /*!< number of bytes processed  */
    uint32_t      state[5];   /*!< intermediate digest state  */
    unsigned char buffer[64]; /*!< data block being processed */
} iot_sha1_context;

/**
 * \brief          Initialize SHA-1 context
 *
 * \param ctx      SHA-1 context to be initialized
 */
void utils_sha1_init(iot_sha1_context *ctx);

/**
 * \brief          Clear SHA-1 context
 *
 * \param ctx      SHA-1 context to be cleared
 */
void utils_sha1_free(iot_sha1_context *ctx);

/**
 * \brief          Clone (the state of) a SHA-1 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void utils_sha1_clone(iot_sha1_context *dst, const iot_sha1_context *src);

/**
 * \brief          SHA-1 context setup
 *
 * \param ctx      context to be initialized
 */
void utils_sha1_starts(iot_sha1_context *ctx);

/**
 * \brief          SHA-1 process buffer
 *
 * \param ctx      SHA-1 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void utils_sha1_update(iot_sha1_context *ctx, const unsigned char *input, size_t ilen);

/**
 * \brief          SHA-1 final digest
 *
 * \param ctx      SHA-1 context
 * \param output   SHA-1 checksum result
 */
void utils_sha1_finish(iot_sha1_context *ctx, unsigned char output[20]);

/* Internal use */
void utils_sha1_process(iot_sha1_context *ctx, const unsigned char data[64]);

/**
 * \brief          Output = SHA-1( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-1 checksum result
 */
void utils_sha1(const unsigned char *input, size_t ilen, unsigned char output[20]);

#endif  // QCLOUD_BLE_LLSYNC_BLE_QIOT_SHA1_H
