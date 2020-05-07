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

#ifndef __AT_RING_BUFF_H__
#define __AT_RING_BUFF_H__

#include "stdbool.h"
#include "stdint.h"

#define RINGBUFF_OK        0  /* No error, everything OK. */
#define RINGBUFF_ERR       -1 /* Out of memory error.     */
#define RINGBUFF_EMPTY     -3 /* Timeout.                     */
#define RINGBUFF_FULL      -4 /* Routing problem.          */
#define RINGBUFF_TOO_SHORT -5

typedef struct _ring_buff_ {
    uint32_t size;
    uint32_t readpoint;
    uint32_t writepoint;
    char*    buffer;
    bool     full;
} sRingbuff;

typedef sRingbuff* ring_buff_t;

int ring_buff_init(sRingbuff* ring_buff, char* buff, uint32_t size);
int ring_buff_flush(sRingbuff* ring_buff);
int ring_buff_push_data(sRingbuff* ring_buff, uint8_t* pData, int len);
int ring_buff_pop_data(sRingbuff* ring_buff, uint8_t* pData, int len);
#endif  // __ringbuff_h__
