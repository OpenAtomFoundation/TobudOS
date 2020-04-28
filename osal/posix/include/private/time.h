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

#ifndef _POSIX_PRIVATE_TIME_H_
#define  _POSIX_PRIVATE_TIME_H_

#include "tos_k.h"

__CDECLS_BEGIN

#define MILLISECOND_PER_SECOND          1000UL
#define MICROSECOND_PER_SECOND          1000000UL
#define NANOSECOND_PER_SECOND           1000000000UL

__KNL__ k_tick_t timespec_to_ktick(const struct timespec *tp);

__KNL__ void ktick_to_timespec(k_tick_t ktick, struct timespec *tp);

__CDECLS_END

#endif /* _POSIX_PRIVATE_TIME_H_ */

