/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */



#ifndef QCLOUD_IOT_UTILS_MD5_H_
#define QCLOUD_IOT_UTILS_MD5_H_

#include "qcloud_iot_import.h"

typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
} iot_md5_context;


 /**
 * @brief init MD5 context
 *
 * @param ctx   MD5 context
 */
void utils_md5_init(iot_md5_context *ctx);

/**
 * @brief free MD5 context
 *
 * @param ctx   MD5 context
 */
void utils_md5_free(iot_md5_context *ctx);

/**
 * @brief clone MD5 context
 *
 * @param dst   destination MD5 context
 * @param src   source MD5 context
 */
void utils_md5_clone(iot_md5_context *dst,
                     const iot_md5_context *src);

/**
 * @brief start MD5 calculation
 *
 * @param ctx   MD5 context
 */
void utils_md5_starts(iot_md5_context *ctx);

/**
 * @brief MD5 update
 *
 * @param ctx MD5 context
 * @param input    input data
 * @param ilen     data length
 */
void utils_md5_update(iot_md5_context *ctx, const unsigned char *input, size_t ilen);

/**
 * @brief          finish MD5 calculation
 *
 * @param ctx      MD5 context
 * @param output   MD5 result
 */
void utils_md5_finish(iot_md5_context *ctx, unsigned char output[16]);

/* MD5 internal process */
void utils_md5_process(iot_md5_context *ctx, const unsigned char data[64]);

/**
 * @brief          Output = MD5( input buffer )
 *
 * @param input    data input
 * @param ilen     data length
 * @param output   MD5 result
 */
void utils_md5(const unsigned char *input, size_t ilen, unsigned char output[16]);


int8_t utils_hb2hex(uint8_t hb);


#endif

