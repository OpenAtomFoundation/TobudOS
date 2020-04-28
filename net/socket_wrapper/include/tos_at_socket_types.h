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

#ifndef  _TOS_AT_SOCKET_TYPES_H_
#define  _TOS_AT_SOCKET_TYPES_H_

#include "stdint.h"

typedef uint8_t     u8_t;
typedef uint16_t    u16_t;
typedef uint32_t    u32_t;

typedef int8_t      s8_t;
typedef int16_t     s16_t;
typedef int32_t     s32_t;

#if !defined(in_addr_t)
typedef u32_t in_addr_t;
#endif

struct in_addr {
    in_addr_t   s_addr;
};

struct sockaddr_in {
    u8_t        sin_len;
    u8_t        sin_family;
    u16_t       sin_port;
    struct      in_addr sin_addr;
    char        sin_zero[8];
};

struct sockaddr {
    u8_t        sa_len;
    u8_t        sa_family;
    char        sa_data[14];
};

#ifndef socklen_t
#  define socklen_t int
#endif

#endif /* _TOS_AT_SOCKET_TYPES_H_ */

