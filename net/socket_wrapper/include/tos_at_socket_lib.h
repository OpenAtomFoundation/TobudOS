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

#ifndef  _TOS_AT_SOCKET_LIB_H_
#define  _TOS_AT_SOCKET_LIB_H_

#include "tos_k.h"
#include "tos_at_socket_types.h"

#if (TOS_CFG_CPU_BYTE_ORDER == CPU_BYTE_ORDER_BIG_ENDIAN)
#ifndef htons
#define htons(x)            (x)
#endif

#ifndef ntohs
#define ntohs(x)            (x)

#endif

#ifndef htonl
#define htonl(x)            (x)
#endif

#ifndef ntohl
#define ntohl(x)            (x)
#endif

#elif (TOS_CFG_CPU_BYTE_ORDER == CPU_BYTE_ORDER_LITTLE_ENDIAN)
#ifndef htons
#define htons(x)            ((u16_t)((((x) & (u16_t)0x00ffU) << 8) | (((x) & (u16_t)0xff00U) >> 8)))
#endif

#ifndef ntohs
#define ntohs(x)            htons(x)
#endif

#ifndef htonl
#define htonl(x)            ((((x) & (u32_t)0x000000ffUL) << 24) | \
                            (((x) & (u32_t)0x0000ff00UL) <<  8) | \
                            (((x) & (u32_t)0x00ff0000UL) >>  8) | \
                            (((x) & (u32_t)0xff000000UL) >> 24))
#endif

#ifndef ntohl
#define ntohl(x)            htonl(x)
#endif

#endif

#define bzero(s, n)         memset(s, 0, n)

#define INADDR_NONE         ((u32_t) 0xffffffff)  /* 255.255.255.255 */

#define in_range(c, lo, up)  ((u8_t)c >= lo && (u8_t)c <= up)
#define isascii(c)           in_range(c, 0x20, 0x7f)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')

u32_t inet_addr(const char *cp);

#endif /* _TOS_AT_SOCKET_LIB_H_ */

