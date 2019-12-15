/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __GLUE_H__
#define __GLUE_H__

#include "os/endian.h"

#define u8_t    uint8_t
#define s8_t    int8_t
#define u16_t   uint16_t
#define u32_t   uint32_t
#define s32_t   int32_t

#ifndef BIT
#define BIT(n)  (1UL << (n))
#endif

#define __packed    __attribute__((__packed__))

#define sys_le16_to_cpu le16toh

struct bt_data {
    u8_t type;
    u8_t data_len;
    const u8_t *data;
};

#define BT_DATA(_type, _data, _data_len) \
    { \
        .type = (_type), \
        .data_len = (_data_len), \
        .data = (const u8_t *)(_data), \
    }

struct os_mbuf * NET_BUF_SIMPLE(uint16_t size);
void net_buf_simple_init(struct os_mbuf *buf, size_t reserve_head);
void net_buf_simple_add_le16(struct os_mbuf *om, uint16_t val);
void net_buf_simple_add_u8(struct os_mbuf *om, uint8_t val);
void *net_buf_simple_add(struct os_mbuf *om, uint8_t len);
uint8_t *net_buf_simple_push(struct os_mbuf *om, uint8_t len);

#define net_buf_simple_add_mem(a,b,c) os_mbuf_append(a,b,c)

const char *bt_hex(const void *buf, size_t len);

#endif /* __GLUE_H__ */
