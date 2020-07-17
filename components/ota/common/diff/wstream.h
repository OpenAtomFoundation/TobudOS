/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _WSTREAM_H_
#define  _WSTREAM_H_

#include "stdint.h"

typedef struct write_stream_st {
    int         buf_size;
    uint8_t    *buf;
    int         cursor;
} wstream_t;

#define WSTREAM_SANITY_CHECK(wstream)       \
    if (!wstream || !wstream->buf) {        \
        return -1;                          \
    }

#define WSTREAM_SANITY_CHECK_RC(wstream, rc)    \
    if (!wstream || !wstream->buf) {            \
        return rc;                              \
    }

int wstream_create(wstream_t *wstream, uint8_t *buf, int buf_size);

int wstream_destroy(wstream_t *wstream);

int wstream_reset(wstream_t *wstream);

int wstream_write_byte(wstream_t *wstream, uint8_t byte);

int wstream_write_byte_at(wstream_t *wstream, int offset, uint8_t byte);

int wstream_write_stream(wstream_t *wstream, uint8_t *stream, int stream_size);

int wstream_write_stream_at(wstream_t *wstream, int offset, uint8_t *stream, int stream_size);

int wstream_length_get(wstream_t *wstream);

uint8_t *wstream_buf_get(wstream_t *wstream);

#endif /*_WSTREAM_H_ */

