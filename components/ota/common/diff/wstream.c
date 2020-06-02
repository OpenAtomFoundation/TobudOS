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

#include "stdlib.h"

#include "wstream.h"

int wstream_create(wstream_t *wstream, uint8_t *buf, int buf_size)
{
    if (!wstream || !buf) {
        return -1;
    }

    wstream->buf        = buf;
    wstream->buf_size   = buf_size;
    wstream->cursor     = 0;

    return 0;
}

int wstream_destroy(wstream_t *wstream)
{
    WSTREAM_SANITY_CHECK(wstream);

    wstream->buf        = (uint8_t *)0;
    wstream->buf_size   = 0;
    wstream->cursor     = 0;
    return 0;
}

int wstream_reset(wstream_t *wstream)
{
    WSTREAM_SANITY_CHECK(wstream);

    wstream->cursor     = 0;
    return 0;
}

int wstream_write_byte(wstream_t *wstream, uint8_t byte)
{
    WSTREAM_SANITY_CHECK(wstream);

    if (wstream->buf_size ==  wstream->cursor) {
        return -1;
    }

    wstream->buf[wstream->cursor++] = byte;

    return 0;
}

int wstream_write_byte_at(wstream_t *wstream, int offset, uint8_t byte)
{
    WSTREAM_SANITY_CHECK(wstream);

    if (offset >= wstream->buf_size) {
        return -1;
    }

    wstream->buf[offset] = byte;

    return 0;
}

int wstream_write_stream(wstream_t *wstream, uint8_t *stream, int stream_size)
{
    int i = 0;

    WSTREAM_SANITY_CHECK(wstream);

    if (wstream->buf_size - wstream->cursor < stream_size) {
        return -1;
    }

    for (i = 0; i < stream_size; ++i) {
        wstream_write_byte(wstream, stream[i]);
    }

    return 0;
}

int wstream_write_stream_at(wstream_t *wstream, int offset, uint8_t *stream, int stream_size)
{
    int i = 0;

    WSTREAM_SANITY_CHECK(wstream);

    if (wstream->buf_size - offset < stream_size) {
        return -1;
    }

    for (i = 0; i < stream_size; ++i) {
        wstream_write_byte_at(wstream, offset++, stream[i]);
    }

    return 0;
}

int wstream_length_get(wstream_t *wstream)
{
    WSTREAM_SANITY_CHECK(wstream);

    return wstream->cursor;
}

uint8_t *wstream_buf_get(wstream_t *wstream)
{
    WSTREAM_SANITY_CHECK_RC(wstream, NULL);

    return wstream->buf;
}

